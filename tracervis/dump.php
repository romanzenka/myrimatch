<html>
<head><title>Myrimatch dump</title></head>
<body>
<table>
<tr><th>Operation</th><th>Started</th><th>Ended</th></tr>
<tbody>
<?php


$db = new SQlite3("test.sq3", SQLITE3_OPEN_READONLY);

function code_ref($id) {
	global $db;
	$res = $db->querySingle("SELECT file, line FROM code WHERE code_id = $id;", TRUE);
	$file = $res['file'];
	$line = $res['line'];
	return "<a href=\"source.php?file=$file&line=$line\">$file($line)</a>";
}

function get_object($id) {
	
}

$opdepth[0] = 0;

$result = $db->query('SELECT operation_id, name, parent_id, code_start_id, code_end_id, terminated_time FROM operation ORDER BY operation_id ASC;');
while($row = $result->fetchArray()) {
	$depth = $opdepth[$row['parent_id']]+1;
	$opdepth[$row['operation_id']] = $depth;
	$prefix = str_repeat('&nbsp&mdash;', $depth);
	echo "<tr><td>" . $prefix . $row['name'] . "</td><td>" . code_ref($row['code_start_id']) . "</td><td>" .code_ref($row['code_end_id']) ."</td></tr>\n";
	$iosresult = $db->query('SELECT io_id, object_id, value, readwrite, note, code_id FROM io where operation_id = '.$row['operation_id'] . " ORDER BY io_id ASC");
	while($ios = $iosresult->fetchArray()) {
		$direction = $ios['readwrite'] == 1 ? '&lt--' : '--&gt;';
		$value = $ios['value'];
		$note = $ios['note'];
		echo "<tr><td>$prefix $direction <tt>$value</tt></td><td>$note</td><td>".code_ref($ios['code_id'])."</td></tr>\n";
	}	
}

$db->close();
?>
</tbody>
</table>
</body>
</html>
