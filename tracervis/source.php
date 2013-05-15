<html>
<head>
<?php

$file = $_GET['file'];
$line = intval($_GET['line']);
$first = isset($_GET['first']) ? intval($_GET['first']) : -1;
$last = isset($_GET['last']) ? intval($_GET['last']) : -1;

$myrimatch_root = '..';

?>
	<title>Myrimatch source: <?php echo("$file ($line)"); ?></title>
</head>
<body>
	<h1><?php echo("$file ($line)"); ?></h1>
<pre>
<?php
$f = file("$myrimatch_root/$file");

foreach ($f as $linenum => $text) {
	$l = $linenum+1;
	echo("<a name=\"$l\">$l</a> ");
	$style = '';
	if($l >= $first && $l <= $last) {
		$style = 'background: #dff';
	}
	if($l == $line) {
		$style = 'background: #ff8';
	}
	if ($style <> '') {
		echo "<span style=\"$style\">";
	}
	echo(htmlspecialchars($text));
	if ($style <> '') {
		echo("</span>");
	}
}

?>
	</pre>
</body>
</html>
