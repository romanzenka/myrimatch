<!DOCTYPE html>
<html lang="en">
<head><title>Myrimatch Object</title></head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link href="bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen">
<link href="css/tracervis.css" rel="stylesheet" media="screen">

<body>
<div style="margin: 1em 0.5em">
<?php
include_once "dao.php";
include_once "render.php";

dao_open();

$object_id = isset($_GET['id']) ? intval($_GET['id']) : 0;
$operation_id  = isset($_GET['operation_id']) ? intval($_GET['operation_id']) : -1;

render_object($object_id, $operation_id);

dao_close();

?>
</div>
<script src="bootstrap/js/bootstrap.min.js"></script>
</body>
</html>
