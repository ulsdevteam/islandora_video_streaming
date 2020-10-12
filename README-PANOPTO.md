# Installation and Setup for Panopto Module

## Description
This file describes installation instructions for the dependencies needed within the Panopto implementation.

## Instructions
1. Download the PHP Panopto Upload API client from: https://github.com/ulsdevteam/php-panopto-upload-api/releases

2. Extract the archive into /sites/all/libraries 

# Panopto API Credential Setup

## Actions Taken in Panopto

For API Client ID : 
   
   1. Click on the Name on the uppermost right of the website to enter 'User Settings'
   
   2. Click on 'API Clients' and 'Create a new API Client' to create a new API client.
   
   3. Enter client information:
        
        Client Name: Enter a name that will help you identify this client in the future.
        
        Client URL: This is an optional field where you can enter the website URL for the client.
        
        Client Type: Select User Based Server Application.


For Folder ID : 
    To create a new folder:
        
        1. Click on 'My Folder" and then 'Add Folder'.
        
        2. Go into new folder and click 'Folder Settings' icon on the right. Then go to 'Manage'.
    
    To use existing folder: 
       
       1. Go to preferred folder and click 'Folder Settings' icon on the right. Then go to 'Manage'.

## Actions Taken in Islandora: 
After enabling the module, select the Islandora menu and then 'Islandora Viewers'. In this section, enter 'Panopto Video Streaming' to enter credentials and server information.

