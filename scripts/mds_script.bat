@echo off
set db_dir=C:\users\jonotron\Documents\image_banks\Caltech_50_cats_10_img_n
set num_entries=500
@echo on
java -jar ..\mds\mdsj.jar -d2 %db_dir%\emd_dist_mat.csv %db_dir%\mds_2d.txt
java -jar ..\mds\mdsj.jar -d3 %db_dir%\emd_dist_mat.csv %db_dir%\mds_3d.txt
java -jar ..\mds\mdsj.jar -d4 %db_dir%\emd_dist_mat.csv %db_dir%\mds_4d.txt
java -jar ..\mds\mdsj.jar -d5 %db_dir%\emd_dist_mat.csv %db_dir%\mds_5d.txt
python ..\mds\dist.py %db_dir%\mds_2d.txt %db_dir%\mds_dist_2d.txt 2
python ..\mds\dist.py %db_dir%\mds_3d.txt %db_dir%\mds_dist_3d.txt 3
python ..\mds\dist.py %db_dir%\mds_4d.txt %db_dir%\mds_dist_4d.txt 4
python ..\mds\dist.py %db_dir%\mds_5d.txt %db_dir%\mds_dist_5d.txt 5
python ..\mds\matcompare.py %db_dir%\emd_dist_mat.csv %db_dir%\mds_dist_2d.txt %num_entries%
python ..\mds\matcompare.py %db_dir%\emd_dist_mat.csv %db_dir%\mds_dist_3d.txt %num_entries%
python ..\mds\matcompare.py %db_dir%\emd_dist_mat.csv %db_dir%\mds_dist_4d.txt %num_entries%
python ..\mds\matcompare.py %db_dir%\emd_dist_mat.csv %db_dir%\mds_dist_5d.txt %num_entries%
