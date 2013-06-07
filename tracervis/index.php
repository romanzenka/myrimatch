<!DOCTYPE html>
<html lang="en">
<head><title>Myrimatch</title></head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link href="bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen">
<link href="css/tracervis.css" rel="stylesheet" media="screen">

<?php
include_once "dao.php";
include_once "summary.php";

dao_open();

?>

<body>
<div style="margin: 1em 0.5em">
<h2>Myrimatch</h2>

<ul>
<li><a href="input_parameters.php">Input Parameters</a></li>
<li><a href="spectra.php">Spectra</a></li>
<li><a href="proteins.php">Proteins</a></li>
<li><a href="results.php">Results</a></li>
</ul>
<ul>
<li><a href="dump.php">Detailed execution</a></li>
<li><a href="all_ops.php">All operations</a></li>
</ul>
</div>
<script src="bootstrap/js/bootstrap.min.js"></script>
</body>
<?php
dao_close();
?>
</html>
