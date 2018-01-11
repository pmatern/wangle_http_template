#!/env/sh
app_name=$1
sed -i -e "s/__REPLACE__/$app_name/g" CMakeLists.txt
sed -i -e "s/__REPLACE__/$app_name/g" .gitignore
