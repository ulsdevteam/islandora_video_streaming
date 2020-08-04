# AWS SDK Installation for Panopto Module

## Description
This file describes installation instructions for the AWS library needed within the Panopto implementation files.

## Instructions
1. Download the AWS SDK from https://docs.aws.amazon.com/aws-sdk-php/v3/download/aws.zip

2. Extract zip into /sites/all/libraries 

# Panopto API Credential Setup

## Actions Taken in Islandora: 
For localhost, enter Islandora tab and then 'Islandora Viewers'. In this section, enter 'Panopto Video Streaming' to enter credentials.

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