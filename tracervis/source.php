<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8"/>
<?php

$file = $_GET['file'];
$line = intval($_GET['line']);
$first = isset($_GET['first']) ? intval($_GET['first']) : -1;
$last = isset($_GET['last']) ? intval($_GET['last']) : -1;
$code = isset($_GET['code']) ? $_GET['code'] : '';

$myrimatch_root = 'myrimatch';

?>
	<title>Myrimatch source: <?php echo("$file ($line)"); ?></title>
	<script type="text/javascript" src="js/shCore.js"></script>
	<script type="text/javascript" src="js/shBrushCpp.js"></script>
	<link href="css/shCore.css" rel="stylesheet" type="text/css" />
	<link href="css/shThemeDefault.css" rel="stylesheet" type="text/css" />
</head>
<body>
<?php
$f = file("$myrimatch_root/$file");

$in_section = 0;

# How many lines above the highlighted line do we show top of the screen
$offset = min(10, $line);

function close_section() {
	global $in_section;
	if($in_section==1) {
		echo "</pre>";
		$in_section = 0;
	}
}

foreach ($f as $linenum => $text) {
	$l = $linenum+1;
	if($l+$offset == $line) {
		close_section();
		echo("<a name=\"$line\"></a>");
	}
	if($l == $first) {
			close_section();
			echo "<div style=\"background: #eff !important\">";
	}
	if($l-1 == $last) {
		close_section();
		echo "</div>";
	}
	if($in_section == 0) {
		echo("<pre class=\"brush: cpp; highlight: [$line], first-line: $l, tab-size: 8, smart-tabs: false\">\n");
		$in_section = 1;
	}
    # Add little pointers around variables (not sure how to do background highlight
    $unicodeChar = '\u261b'; $rightArrow = json_decode('"'.$unicodeChar.'"');
    $unicodeChar = '\u261a'; $leftArrow = json_decode('"'.$unicodeChar.'"');
    if($code != '') {
        $text = preg_replace('/\b\Q' . $code . '\E\b/', $rightArrow . $code . $leftArrow, $text);
    }
	echo('&#8203;'.htmlspecialchars($text));
}

?>
	</pre>
<script type="text/javascript">
	SyntaxHighlighter.defaults['toolbar']=false;
	SyntaxHighlighter.all();
</script>
</body>
</html>
