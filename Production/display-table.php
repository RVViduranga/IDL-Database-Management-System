<!DOCTYPE html>
<html>
<head>
    <title>IDL Production Dashboard</title>
    <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
    <h1>IDL Production Dashboard</h1>
    <a href="index.php" class="back-button">Back</a>

    <?php
    $machineNames = array("Machine 1", "Machine 2", "Machine 3"); // Define machine names

    // Get the selected date from the previous page
    $selectedDate = $_POST['selected_date'];
    $machine1 = $_POST['machine_1'];
    $machine2 = $_POST['machine_2'];
    $machine3 = $_POST['machine_3'];

    $machineTargets = array(
        "Machine 1" => $machine1,
        "Machine 2" => $machine2,
        "Machine 3" => $machine3
    );

    // Connect to your MySQL database
    $servername = "localhost";
    $username = "root";
    $password = "";
    $dbname = "idlnew";

    $conn = new mysqli($servername, $username, $password, $dbname);

    // Check the connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    // Function to update the table using AJAX
    function updateTable($selectedDate, $machineNames, $machineTargets, $conn) {
        // Retrieve data from the database for the selected date
        $sql = "SELECT sensor, COUNT(*) as total_count FROM sensordata WHERE DATE(reading_time) = '$selectedDate' GROUP BY sensor";
        $result = $conn->query($sql);

        // Initialize arrays for infeed and outfeed counts
        $infeedCounts = array_fill_keys($machineNames, 0);
        $outfeedCounts = array_fill_keys($machineNames, 0);

        // Update counts based on the retrieved data
        while ($row = $result->fetch_assoc()) {
            foreach ($machineNames as $machineName) {
                $infeedSensor = $machineName == "Machine 1" ? "0001" : ($machineName == "Machine 2" ? "0003" : ($machineName == "Machine 3" ?  "0005" : ""));

                $outfeedSensor = $machineName == "Machine 1" ? "0002" : ($machineName == "Machine 2" ? "0004" : ($machineName == "Machine 3" ?  "0006" : ""));

                if ($row['sensor'] === $infeedSensor) {
                    $infeedCounts[$machineName] = $row['total_count'];
                } elseif ($row['sensor'] === $outfeedSensor) {
                    $outfeedCounts[$machineName] = $row['total_count'];
                }
            }
        }

        // Close the database connection
        $conn->close();

        // Return the updated table HTML
        $tableHTML = '<table border="1">';
        $tableHTML .= '<tr><th>Machine Name</th><th>Bottle In Count</th><th>Finish Product Count</th><th>Target</th></tr>';
        foreach ($machineNames as $machineName) {
            $tableHTML .= "<tr><td>$machineName</td><td>" . $infeedCounts[$machineName] . "</td><td>" . $outfeedCounts[$machineName] . "</td><td>" . $machineTargets[$machineName] . "</td></tr>";
        }
        $tableHTML .= '</table>';

        return $tableHTML;
    }

    // Display the table
    echo updateTable($selectedDate, $machineNames, $machineTargets, $conn);
    ?>

    <div id="table-container">
        <!-- The updated table will be displayed here -->
    </div>

    <!-- JavaScript to refresh the table using AJAX -->
    <script src="http://localhost/Production/jquery-3.6.0.min.js"></script>
    <script>
        $(document).ready(function() {
            setInterval(function() {
                $.ajax({
                    type: 'GET',
                    url: 'C:/Apache24/htdocs/new/display-table.php', // Replace with the actual page URL where this PHP code is located
                    success: function(data) {
                        $('#table-container').html(data);
                    }
                });
            }, 1000); // Refresh the table every 1 seconds
        });
    </script>
    <!-- JavaScript to refresh the page automatically every 5 seconds -->
    <script>
        $(document).ready(function() {
            setInterval(function() {
                location.reload();
            }, 1000); // Refresh the page every 1 seconds
        });
    </script>
    <div class="date-display">
        <p><?php echo $selectedDate; ?></p>
    </div>
</body>
</html>
