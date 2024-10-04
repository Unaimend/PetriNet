library(shiny)
library(shinyjs)
library(jsonlite)
library(ggplot2)
library(tidyverse)
# Function to read the JSON files and combine them into a data frame
load_data <- function() {
  data_list <- list()
  
  # Loop through your 10 JSON files and read them into a list
  for (i in 1:12) {
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
  for (i in 1:12) {
    file_name <- paste0("TH_", i, ".json")
    data <- fromJSON(file_name)
    data_list[[i]] <- data
  }
  
  # Combine the data into a data frame
  return(data_list)
}


# Shiny app

ui <- navbarPage("My Application", 
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
                            selectInput("metabolite2", "Select a Metabolite:",  choices = colnames(load_data()), selected = "13dpg_c"),
                            sliderInput("slider2", "Max x value:",  min = 100, max = 1000, value = 500)
                            
                          ),
                        ),
                        mainPanel(
                         fluidPage(
                           plotOutput("scatterPlot")
                          )
                        )
                      )
                    ),
                 tabPanel("Component 2")
)

server <- function(input, output) {
  # Load data once when the app starts
  data <- load_data()
  data2 <- as.data.frame(load_data2()[[1]], check.names = FALSE)
  print(data2)

  
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

  output$scatterPlot<- renderPlot({
    selected_metabolite <- input$metabolite2
    print(selected_metabolite)
    df_plot <- pivot_longer(data2,names_to = "metabolite", values_to = "concentration", cols = everything())
    df_plot <- df_plot %>% group_by(metabolite) %>%
       mutate(rank = row_number()) %>%
       ungroup()
    df_plot <- filter(df_plot, metabolite == selected_metabolite)
    df_plot <- filter(df_plot, rank < input$slider2)
    df_plot$concentration <- as.numeric(df_plot$concentration)
    # Plot the selected metabolite across all 10 files
    ggplot(df_plot, aes(x = c(1:nrow(df_plot)), y = concentration, color = metabolite)) +
      geom_point() +
      labs(x = "File", y = "Concentration", 
           title = paste("Concentration of", selected_metabolite)) +
      theme_minimal()
  })


}

# Run the app
shinyApp(ui = ui, server = server)
#server()
Sys.sleep(200)
