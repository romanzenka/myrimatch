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
	<script type="text/javascript" src="js/shCore.js"></script>
	<script type="text/javascript" src="js/shBrushCpp.js"></script>
	<link href="css/shCore.css" rel="stylesheet" type="text/css" />
	<link href="css/shThemeDefault.css" rel="stylesheet" type="text/css" />
</head>
<body>
	<h1><?php echo("$file ($line)"); ?></h1>
<?php
$f = file("$myrimatch_root/$file");

$in_section = 0;

foreach ($f as $linenum => $text) {
	$l = $linenum+1;
	if($l == $line && $in_section==1) {
		echo("</pre>");
		$in_section = 0;
		echo("<a name=\"$line\"></a>");
	}
	if($in_section == 0) {
		echo("<pre class=\"brush: cpp; highlight: [$first, $last, $line], first-line: $l\">");
		$in_section = 1;
	}
	echo(htmlspecialchars($text));
}

?>
	</pre>
<script type="text/javascript">
	SyntaxHighlighter.defaults['toolbar']=false;
	SyntaxHighlighter.all();
</script>
</body>
</html>
