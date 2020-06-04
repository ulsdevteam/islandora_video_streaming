#AWS SDK Installation for Panopto Module

##Description
This file describes installation instructions for the AWS library needed within the Panopto implementation files.

##Instructions
1. Download the AWS SDK from https://docs.aws.amazon.com/aws-sdk-php/v3/download/aws.zip

2. Extract zip into /sites/all/libraries 

3. Create an info file within /sites/all/libraries named "aws.libraries.info". 
    Info file should contain: 
    name = aws
    machine name = aws
    description = Provides aws api for Panopto
    version = 3
    files[php][] =aws-autoloader.php

    *note info file should not be inside /sites/all/libraries/aws

4. Add "dependencies[] = libraries" to "islandora_video_streaming_panopto.info" (and any other module info file using AWS SDK)

5. Insert "libraries_load('aws')" to the beginning of code file which uses it.
