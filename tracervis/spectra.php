<!DOCTYPE html>
<html lang="en">
<head><title>Spectra processed | Myrimatch</title></head>
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
<h2><?php home_btn(); ?> Spectra</h2>

<?php summary_spectra() ?>

</div>
<script src="bootstrap/js/bootstrap.min.js"></script>
</body>
<?php
dao_close();
?>
</html>
