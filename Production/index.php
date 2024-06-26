<!DOCTYPE html>
<html>
<head>
    <title>Date Entry</title>
    <link rel="stylesheet" type="text/css" href="styles.css">
</head>
<body>
    <form method="post" action="display-table.php">
        Enter Date (yyyy-mm-dd): <input type="text" name="selected_date" pattern="\d{4}-\d{2}-\d{2}" placeholder="yyyy-mm-dd">
        Machine 1 : <input type="text" name="machine_1">
        Machine 2 : <input type="text" name="machine_2">
        Machine 3 : <input type="text" name="machine_3">
        <input type="submit" value="Submit">
    </form>
</body>
</html>
