library(shiny)
library(shinyjs)
library(jsonlite)
library(ggplot2)
library(tidyverse)
THREADS <- 2
# Func tion to read the JSON files and combine them into a data frame

load_data_int <- function(prefix) {
  data_list <- list()
  # Loop through your 10 JSON files and read them into a list
  for (i in 1:THREADS) {
    file_name <- paste0(prefix, i, ".json")
    data <- fromJSON(file_name)
    data_list[[i]] <- data
  }
  return(data_list)
}

load_data <- function() {
  data_list <- load_data_int("FTC_")
  df <- do.call(rbind, data_list)
  return(df %>% as_tibble())
}


load_token_history <- function() {
  data_list <- load_data_int("TH_")

  # Token history is a json file that contains a list for each metabolite
  data2 <- data.frame(metabolite = character(), concentration = numeric(), run = factor())

  for(i in 1:THREADS) { 
    # We first convert each list to a df and then make it longer
    data_temp <- as.data.frame(data_list[[i]], check.names = FALSE)
    df_plot <- pivot_longer(data_temp, names_to = "metabolite", values_to = "concentration", cols = everything())
    # We add the run it came from
    df_plot$run <- as.factor(i)
    # and just append it two the full df
    data2 <- rbind(data2, df_plot)
  }

  return(data2)
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
              selectInput("metabolite", "Select a Metabolite:",
                          choices = colnames(load_data()), selected = "13dpg_c")),
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
                        choices = c(colnames(load_data()), "all"), selected = "13dpg_c", multiple = TRUE), 
            selectInput("item2", "Select a run:",
                        choices = c(1:length(unique(load_token_history()$run)), "all"), selected = "all"), 
            sliderInput("slider2", "Max x value:",
                        min = 0, max = 1000, value = 1000)
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
  token_history_data <- load_token_history()
  

  output$barPlot <- renderPlot({
    selected_metabolite <- input$metabolite
    ggplot(token_history_data %>% filter(metabolite == selected_metabolite), aes(x = metabolite, y = concentration)) +
      geom_boxplot() +
      labs(x = "File", y = "Concentration",
           title = paste("Concentration of", selected_metabolite)) +
      theme_minimal()
  })

  output$scatterPlot <- renderPlot({
    selected_metabolite <- input$metabolite2
    selected_run <- input$item2
    
    token_history_data %>%
      filter(if(selected_metabolite != "all") metabolite %in% selected_metabolite else TRUE) %>%
      group_by(run) %>%
      mutate(rank = row_number()) %>%
      ungroup() %>%
      filter(if(selected_run != "all") run == selected_run else TRUE)  %>%
      filter(rank < input$slider2) %>%
      ggplot(aes(y = concentration, x = rank, color = if (selected_run != "all") metabolite else run)) +
      geom_point() +
      labs(x = "File", y = "Concentration",
           title = paste("Concentration of",
                         selected_metabolite, "of run", selected_run),
           color = "Run"
           ) +
      theme_minimal()
  })
}

# Run the app
shinyApp(ui = ui, server = server)
Sys.sleep(200)
