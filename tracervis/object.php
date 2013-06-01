<!DOCTYPE html>
<html lang="en">
<head><title>Myrimatch Object</title></head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link href="bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen">
<link href="css/tracervis.css" rel="stylesheet" media="screen">

<body>
<div class="container" style="margin-top: 1em">
<?php
include_once "dao.php";
include_once "render.php";

dao_open();

$object_id = isset($_GET['id']) ? intval($_GET['id']) : 0;
$operation_id  = isset($_GET['operation_id']) ? intval($_GET['operation_id']) : -1;

$object = get_object($object_id);
$ios = get_io_for_object($object_id);
$name = get_name_for_object($ios);

echo '<h2><span style="color: #ddd">' . htmlspecialchars($object['type']) . '</span> ' . htmlspecialchars($name) . '</h2>';


echo '<table class="table table-condensed">';
$prev_io = null;
foreach($ios as $io) {
    if($io['operation_id'] == $operation_id) {
        echo "<tr class=\"highlight\">";
    } else {
        echo "<tr>";
    }
    echo '<td>'.$io['id'].'</td>';
    echo render_io($io, 'operation', $name, $prev_io);
    echo "</tr>";
    $prev_io = $io;
}
echo '</table>';

dao_close();

?>
</div>
<script src="bootstrap/js/bootstrap.min.js"></script>
</body>
</html>
