# Islandora Video Streaming

## Overview

Enhances the [Islandora Video solution pack](https://github.com/Islandora/islandora_solution_pack_video/) to enable delivery of streaming video.

Supported streaming video sources include:
* [Panopto](https://www.panopto.com/)
* [Kaltura](https://corp.kaltura.com/)

## Requirements

This module requires an [Islandora 7.x installation](https://islandora.ca/), and the Islandora Video Solution Pack.

A hosted account, or a local install, of one of the streaming video platforms is required.

For Kaltura integration, the [Kaltura Drupal module](https://drupal.org/project/kaltura) is required.

## Description

When enabled, derivatives will be created by uploading video content to the streaming server(s).  A new JSON-based datastream will be created to describe the remote video location.  The viewer for the object will be overridden to display content from the streaming service with the highest module priority.

TODO: describe configuration details

## Copyright and License

Copyright University of Pittsburgh.

Licensed under GPL v2 or later.
