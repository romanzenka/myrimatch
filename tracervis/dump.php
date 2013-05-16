<!DOCTYPE html>
<html lang="en">
<head><title>Myrimatch dump</title></head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<link href="bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen">

<body>
<div class="container" style="margin-top: 1em">
<?php

$operation_id = isset($_GET['id']) ? intval($_GET['id']) : 0;

$db = new SQlite3("test.sq3", SQLITE3_OPEN_READONLY);
# Get operation for a given id
$operation_stmt = $db->prepare("SELECT operation_id, name, parent_id, code_start_id, code_end_id, terminated_time FROM operation WHERE operation_id = :id");
# get a reference to code by reference id
$code_ref_stmt = $db->prepare("SELECT file, line FROM code WHERE code_id = :id");
# Get list of IOs for given operation
$io_stmt = $db->prepare('SELECT io_id, object_id, value, readwrite, note, code_id FROM io where operation_id = :id ORDER BY io_id ASC');

$annotations = load_annotations();

function code_ref_data($id) {
    global $code_ref_stmt;
    $code_ref_stmt->bindParam("id", $id, SQLITE3_INTEGER);
    return $code_ref_stmt->execute()->fetchArray();
}

# Load pretty names and descriptions from a file
function load_annotations() {
    $state = 0;
    $id = '';
    $sdesc = '';
    $desc = '';
    $result = array();
    foreach(file('annotation.txt') as $line) {
        $line = rtrim($line);
        switch($state) {
            case 0:
                $id = $line;
                $state = 1;
                break;
            case 1:
                $sdesc = $line;
                $desc = '';
                $state = 2;
                break;
            case 2:
                if($line == '') {
                    $state = 0;
                    $result[$id] = array('sdesc' => $sdesc, 'desc' => $desc);
                } else {
                    $desc .= $line . "<br/>\n";
                }
                break;
        }
    }
    return $result;
}

# Link to a line in code
function code_ref($id) {
    $res = code_ref_data($id);
 	$file = $res['file'];
	$line = $res['line'];
	return '<a href="source.php?file='."$file&line=$line#$line\">$file($line)</a>";
}

# Link to a code block (has start+end, first line is highlighted)
function code_block_ref($start_id, $end_id) {
    $res = code_ref_data($start_id);
	$file = $res['file'];
	$line = $res['line'];

    $res = code_ref_data($end_id);
	$file_end = $res['file'];
	$line_end = $res['line'];

    if($file_end <> $file) {
        return "<div class=\"error\">Operation ends in a different file</div>";
    }

	return 'source.php'."?file=$file&line=$line&first=$line&last=$line_end#$line";
}

function get_object($id) {
	
}

function get_operation($id) {
    global $operation_stmt, $annotations;

    $operation_stmt->bindParam('id', $id);
    $result = $operation_stmt->execute()->fetchArray();
    if(array_key_exists($result['name'], $annotations)) {
        $annotation = $annotations[$result['name']];
        $result['sdesc'] = $annotation['sdesc'];
        $result['desc'] = $annotation['desc'];
    } else {
        $result['sdesc'] = $result['name'];
        $result['desc'] = '';
    }
    return $result;
}

# Direct children of operation of given id. Returns
function get_operation_children($id) {
    global $db;
    return $db->query("SELECT operation_id FROM operation WHERE parent_id = $id");
}

function get_operation_link($operation) {
    $sdesc = $operation['sdesc'];
    $id = $operation['operation_id'];
    return "<a href=\"dump.php?id=$id\">$sdesc</a>";
}

# Go all the way to the root operation, produce html code for breadcrumbs
function get_breadcrumbs($id) {
    $result = "";
    while($id != -1) {
        $operation = get_operation($id);
        $result = get_operation_link($operation) . '<i class="icon-chevron-right"></i>' . $result;
        $id = $operation['parent_id'];
    }
    return $result;
}

function render_operation($id) {
    $operation = get_operation($id);
    $sdesc = htmlspecialchars($operation['sdesc']);
    $desc = $operation['desc'];
    $parent_id = $operation['parent_id'];
    $code_url = code_block_ref($operation['code_start_id'], $operation['code_end_id']);
    $breadcrumbs = get_breadcrumbs($parent_id);

    echo "<p><a href=\"dump.php?id=$parent_id\" class=\"btn btn-small\"><i class=\"icon-arrow-up\"></i></a><a href=\"$code_url\" class=\"btn btn-small\"><i class=\"icon-tasks\"></i></a> $breadcrumbs <b>$sdesc</b></p>";
    echo "<p>$desc</p>";

    $children = get_operation_children($id);
    while ($child = $children->fetchArray()) {
        $child_operation = get_operation($child['operation_id']);
        echo '<i class="icon-chevron-right"></i> ' . get_operation_link($child_operation);
        echo "<br/>";
    }
}

$opdepth[0] = 0;

render_operation($operation_id);
//
//
//while($row = $result->fetchArray()) {
//	$depth = $opdepth[$row['parent_id']]+1;
//	$opdepth[$row['operation_id']] = $depth;
//	$prefix = str_repeat('&nbsp&mdash;', $depth);
//	echo "<tr><td>" . $prefix . $row['name'] . "</td><td>" . code_block_ref($row['code_start_id'], $row['code_end_id']) ."</td></tr>\n";
//	$iosresult = $db->query('SELECT io_id, object_id, value, readwrite, note, code_id FROM io where operation_id = '.$row['operation_id'] . " ORDER BY io_id ASC");
//	while($ios = $iosresult->fetchArray()) {
//		$direction = $ios['readwrite'] == 1 ? '&lt--' : '--&gt;';
//		$value = $ios['value'];
//		$note = $ios['note'];
//		echo "<tr><td>$prefix $direction <tt>$value</tt></td><td>$note</td><td>".code_ref($ios['code_id'])."</td></tr>\n";
//	}
//}

$db->close();
?>
</div>
<script src="bootstrap/js/bootstrap.min.js"></script>
</body>
</html>
