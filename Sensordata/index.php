<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="refresh" content="5">
    <link rel="stylesheet" type="text/css" href="style.css" media="screen"/>
    <title>IDL SENSOR DATABASE</title>
</head>
<body>
    <h1>IDL SENSOR DATABASE</h1>

    <!-- Save as CSV Button -->
    <button id="saveCsvButton">Save as CSV</button>

    <!-- Reset Database Form -->
    <form method="post" action="">
        <input type="submit" name="reset_database" value="Reset Database" />
    </form>

    <?php
    $servername = "localhost";
    $username = "root";
    $password = "";
    $dbname = "idlnew";

    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    // Reset Database
    if (isset($_POST['reset_database'])) {
    	$sql_reset = "DELETE FROM SensorData";
    	if ($conn->query($sql_reset) === TRUE) {
        	// Reset the auto-increment counter for the ID column
        	$reset_auto_increment = "ALTER TABLE SensorData AUTO_INCREMENT = 1";
        	if ($conn->query($reset_auto_increment) === TRUE) {
            		echo "Database reset successfully.";
        	} else {
            		echo "Error resetting auto-increment counter: " . $conn->error;
        	}
    	} else {
        	echo "Error resetting database: " . $conn->error;
    	}
    }

    // Fetch data in its original order for webpage display
    $sql = "SELECT id, sensor, value1, reading_time FROM sensordata ORDER BY id DESC";

    $dataArray = array(); // Create an array to store data

    if ($result = $conn->query($sql)) {
        while ($row = $result->fetch_assoc()) {
            $dataArray[] = array(
                'ID' => $row["id"],
                'Date & Time' => $row["reading_time"],
                'Sensor' => $row["sensor"],
                'Motion Detection' => $row["value1"]
            );
        }
        $result->free();
    }

    // Create a copy of the data array and sort it in ascending order for CSV export
    $dataArrayCopy = $dataArray;
    usort($dataArrayCopy, function($a, $b) {
        return $a['ID'] - $b['ID'];
    });

    $conn->close();
    ?> 

    <!-- Display Data on Webpage -->
    <table cellspacing="5" cellpadding="5">
        <tr> 
            <th>ID</th> 
            <th>Date & Time</th> 
            <th>Sensor</th> 
            <th>Motion Detection</th> 
        </tr>
        <?php foreach ($dataArray as $row) : ?>
            <tr> 
                <td><?php echo $row['ID']; ?></td> 
                <td><?php echo $row['Date & Time']; ?></td> 
                <td><?php echo $row['Sensor']; ?></td> 
                <td><?php echo $row['Motion Detection']; ?></td> 
            </tr>
        <?php endforeach; ?>
    </table>

    <!-- JavaScript for Saving as CSV -->
    <script>
    document.getElementById("saveCsvButton").addEventListener("click", function () {
        // Create a CSV data string from the sorted dataArrayCopy
        var csv = [];
        csv.push(Object.keys(<?php echo json_encode($dataArrayCopy[0]); ?>).join(',')); // Write CSV header
        <?php foreach ($dataArrayCopy as $row) : ?>
            csv.push(<?php echo json_encode(implode(',', $row)); ?>); // Write data rows
        <?php endforeach; ?>

        var csvContent = "data:text/csv;charset=utf-8," + csv.join('\n');

        // Create a download link for the CSV
        var a = document.createElement('a');
        a.href = encodeURI(csvContent);
        a.download = 'sensor_data.csv'; // Filename for the CSV file
        a.style.display = 'none';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
    });
    </script>
</body>
</html>
