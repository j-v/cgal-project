@echo off 
set db_dir=C:\users\jonotron\Documents\image_banks\Caltech_50_cats_10_img_n
set build_opts=-N
@echo on
..\src\Release\chsemd.exe -builddb %db_dir% -l %db_dir%\build_db_log.csv %build_opts%
..\src\Release\emdmatrix.exe %db_dir%
java -jar ..\mds\mdsj.jar -d3 %db_dir%\emd_dist_mat.csv %db_dir%\mds_3d.txt
python combine_names_and_points.py %db_dir%\emdindex.csv %db_dir%\mds_3d.txt %db_dir%\names_and_points_3d.txt


