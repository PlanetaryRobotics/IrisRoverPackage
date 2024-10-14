./run-script.sh ./scripts/utils/mission_archive_plotter.py -p "fm1_day" -t "Iris Lunar Rover FM1 - Day "  -s "24:00:00" -d 300 600 800 -e jpg png svg

./run-script.sh ./scripts/utils/mission_archive_plotter.py -p "fm1_halfday" -t "Iris Lunar Rover FM1 - Half-Day "  -s "12:00:00" -d 300 600 800 -e jpg png svg

./run-script.sh ./scripts/utils/mission_archive_plotter.py -p "_fm1_motors_test" -t "Iris Lunar Rover FM1 - Cislunar Motors Tests "  --plot-start-time-utc "2024/1/15T4:52:00" --plot-end-time-utc "2024/1/15T5:25:00" -d 300 600 800 -e jpg png svg

./run-script.sh ./scripts/utils/mission_archive_plotter.py -p "_fm1_motors_test_min" -t "Iris Lunar Rover FM1 - Cislunar Motors Tests "  --plot-start-time-utc "2024/1/15T4:52:00" --plot-end-time-utc "2024/1/15T5:25:00" -d 300 600 -e jpg png svg --no-plot-packets --no-plot-data-use

./run-script.sh ./scripts/utils/mission_archive_plotter.py -p "_fm1_opportunity1" -t "Iris Lunar Rover FM1 - Opportunity 1"  --plot-start-time-utc "2024/1/9T13:50:00" --plot-end-time-utc "2024/1/9T16:00:00" -d 300 600 800 -e jpg png svg

./run-script.sh ./scripts/utils/mission_archive_plotter.py -p "_fm1_opportunity2" -t "Iris Lunar Rover FM1 - Opportunity 2"  --plot-start-time-utc "2024/1/14T23:15:00" --plot-end-time-utc "2024/1/15T05:45:00" -d 300 600 800 -e jpg png svg

./run-script.sh ./scripts/utils/mission_archive_plotter.py -p "_fm1_mission" -t "Iris Lunar Rover FM1 - Full Mission"  -d 300 600 800 -e jpg png svg