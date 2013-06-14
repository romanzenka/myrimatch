<!DOCTYPE html>
<html lang="en">
<head><title>Search Results | Myrimatch</title></head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link href="bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen">
<link href="css/tracervis.css" rel="stylesheet" media="screen">

<?php
$spectrum_id = isset($_GET['spectrum_id']) ? $_GET['spectrum_id'] : 0;
$protein_name = isset($_GET['protein_name']) ? $_GET['protein_name'] : 0;

include_once "dao.php";
include_once "summary.php";
dao_open();
?>

<body>
<div style="margin: 1em 0.5em">
<h2><?php home_btn(); ?> Results</h2>

<?php summary_search_results($spectrum_id, $protein_name) ?>

</div>
<script src="bootstrap/js/bootstrap.min.js"></script>
</body>
<?php
dao_close();
?>
</html>

