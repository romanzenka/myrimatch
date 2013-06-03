<!DOCTYPE html>
<html lang="en">
<head><title>Myrimatch Operation</title></head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link href="bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen">
<link href="css/tracervis.css" rel="stylesheet" media="screen">

<body>
<div style="margin: 1em 0.5em">
<?php
include_once "dao.php";
include_once "render.php";

dao_open();

$operation_id = isset($_GET['id']) ? intval($_GET['id']) : 0;
$highlight_object_id = isset($_GET['object_id']) ? intval($_GET['object_id']) : 0;

render_operation($operation_id, $highlight_object_id);

dao_close();
?>
</div>
<script src="bootstrap/js/bootstrap.min.js"></script>
</body>
</html>
