<?php

/**
 * @file
 * This is the template file for embedding Panopto iframe videos.
 *
 * Available variables: *EDIT*
 * - $islandora_content: The rendered output of the viewer configured for
 *   this module.
 * - $islandora_dublin_core: The DC datastream object
 * - $dc_array: The DC datastream object values as a sanitized array. This
 *   includes label, value and class name.
 * - $islandora_object_label: The sanitized object label.
 * - $parent_collections: An array containing parent collection(s) info.
 *   Includes collection object, label, url and rendered link.
 *
 */
?>
<iframe src="https://pitt.hosted.panopto.com/Panopto/Pages/Embed.aspx?id=<?php print $url?>
&autoplay=<?php print htmlentities($autoplay)?>
&offerviewer=<?php print htmlentities($offer_viewer)?>
&showtitle=<?php print htmlentities($showtitle)?>
&showbrand=<?php print htmlentities($showbrand)?>
&start=<?php print htmlentities($start_at)?>
&interactivity=<?php print htmlentities($interactivity)?>"
width="<?php print htmlentities($width)?>"
height="<?php print htmlentities($height)?>"
style="border: 1px solid #464646;" allowfullscreen allow="autoplay"></iframe>