library(shiny)
library(shinyjs)
library(jsonlite)
library(ggplot2)
library(tidyverse)
library(dplyr)
THREADS <- 1
# Func tion to read the JSON files and combine them into a data frame
load_data <- function() {
  data_list <- list()

  # Loop through your 10 JSON files and read them into a list
  for (i in 1:THREADS) {
    file_name <- paste0("FTC_", i, ".json")
    data <- fromJSON(file_name)
    data_list[[i]] <- data
  }

  # Combine the data into a data frame
  df <- do.call(rbind, data_list)
  return(df)
}


load_data2 <- function() {
  data_list <- list()

  # Loop through your 10 JSON files and read them into a list
  for (i in 1:THREADS) {
    file_name <- paste0("TH_", i, ".json")
    data <- fromJSON(file_name)
    data_list[[i]] <- data
  }

  # Combine the data into a data frame
  return(data_list)
}


# Shiny app

ui <-
  navbarPage("My Application",
    tabPanel("Metabolite Distribution",
      fluidPage(
        useShinyjs(),
        titlePanel("Metabolite Visualization"),
        sidebarLayout(
          sidebarPanel(
            fluidPage(
              selectInput("metabolite", "Select a Metabolite:",  choices = colnames(load_data()), selected = "13dpg_c")),
          ),
          mainPanel(
            fluidPage(
              plotOutput("barPlot")
            )
          )
        )
      )
    ),
    tabPanel("Metabolite History",
      useShinyjs(),
      titlePanel("Metabolite Visualization"),
      sidebarLayout(
        sidebarPanel(
          fluidPage(
            selectInput("metabolite2", "Select a Metabolite:",
                        choices = c(colnames(load_data()), "all"), selected = "13dpg_c", multiple = TRUE), # nolint: line_length_linter.
            selectInput("item2", "Select a run:",
                        choices = c(1:length(load_data2()), "all"), selected = "all"), # nolint: line_length_linter.
            sliderInput("slider2", "Max x value:",
                        min = 100, max = 1000, value = 1000)
          ),
        ),
        mainPanel(
          fluidPage(
            plotOutput("scatterPlot")
          )
        )
      )
    ),
  )

server <- function(input, output) {
  # Load data once when the app starts
  data <- load_data()
  d <- load_data2()
  data2 <- data.frame(metabolite = character(), concentration = numeric(), item = factor())
  for(i in 1:THREADS){ 
    data_temp <- as.data.frame(d[[i]], check.names = FALSE)
    df_plot <- pivot_longer(data_temp ,names_to = "metabolite", values_to = "concentration", cols = everything())
    df_plot$item <- as.factor(i)
    data2 <- rbind(data2, df_plot)
  }
  print(nrow(data2))
  print(unique(data2$item))
  output$barPlot <- renderPlot({
    selected_metabolite <- input$metabolite
    df_plot <- pivot_longer(as.data.frame(data), names_to = "metabolite", values_to = "concentration", cols = everything())
    df_plot <- filter(df_plot, metabolite == selected_metabolite)
    df_plot$concentration <- as.numeric(df_plot$concentration)
    # Plot the selected metabolite across all 10 files
    ggplot(df_plot, aes(x = metabolite, y = concentration)) +
      geom_boxplot() +
      labs(x = "File", y = "Concentration",
           title = paste("Concentration of", selected_metabolite)) +
      theme_minimal()
  })

  output$scatterPlot <- renderPlot({
    selected_metabolite <- input$metabolite2
    if (selected_metabolite != "all") {
      df_plot <- filter(data2, metabolite %in% selected_metabolite)
    } else {
      df_plot <- data2
    }
    selected_run <- input$item2
    df_plot <- df_plot %>%
      group_by(item) %>% # nolint: object_usage_linter.
      mutate(rank = row_number()) %>%
      ungroup()
    print(head(df_plot))
    if (selected_run != "all") {
      df_plot <- filter(df_plot, item == selected_run) # nolint
    }

    df_plot <- filter(df_plot, rank < input$slider2)
    df_plot$concentration <- as.numeric(df_plot$concentration)
    # Plot the selected metabolite across all 10 files
    if (selected_run != "all") {
      p <-  ggplot(df_plot, aes(y = concentration, x = rank, color = metabolite))
    } else {
      p <- ggplot(df_plot, aes(y = concentration, x = rank, color = item))
    }
    p +
      geom_point() +
      labs(x = "File", y = "Concentration",
           title = paste("Concentration of",
                         selected_metabolite, "of run", selected_run)) +
      theme_minimal()
  })


}

# Run the app
shinyApp(ui = ui, server = server)
#server()
Sys.sleep(200)
