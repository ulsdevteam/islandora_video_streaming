<?php

/**
 * @file
 * This is the template file for embedding Panopto iframe videos.
 *
 */
?>
<iframe
src="https://pitt.hosted.panopto.com/Panopto/Pages/Embed.aspx?id=<?php print $url?>&autoplay=<?php print htmlentities($autoplay)?>&offerviewer=<?php print htmlentities($offer_viewer)?>&showtitle=<?php print htmlentities($showtitle)?>&showbrand=<?php print htmlentities($showbrand)?>&start=<?php print htmlentities($start_at)?>&interactivity=<?php print htmlentities($interactivity)?>"
width="<?php print htmlentities($width)?>"
height="<?php print htmlentities($height)?>"
style="border: 1px solid #464646;"
allowfullscreen="true"
allow="autoplay">
</iframe>
