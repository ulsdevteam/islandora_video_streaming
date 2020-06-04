# AWS SDK Installation for Panopto Module

## Description
This file describes installation instructions for the AWS library needed within the Panopto implementation files.

## Instructions
1. Download the AWS SDK from https://docs.aws.amazon.com/aws-sdk-php/v3/download/aws.zip

2. Extract zip into /sites/all/libraries 

3. Add "dependencies[] = libraries" to "islandora_video_streaming_panopto.info" (and any other module info file using AWS library)

4. Insert "libraries_load('aws')" to the beginning of code file which uses it.
