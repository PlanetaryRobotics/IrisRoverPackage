COMPONENT_NAME=$1




OutputPath=./$COMPONENT_NAME
Aixml=${OutputPath}/${COMPONENT_NAME}ComponentAi.xml

mkdir -p ./$COMPONENT_NAME
cp ./CubeRoverComponentTemplate/CubeRoverComponentTemplateAi.xml $Aixml
cp ./CubeRoverComponentTemplate/Makefile $OutputPath
cp ./CubeRoverComponentTemplate/mod.mk $OutputPath
sed -i "s/{{ COMPONENT_NAME }}/$COMPONENT_NAME/g" $Aixml
sed -i "s/{{ COMPONENT_NAME }}/$COMPONENT_NAME/g" ${OutputPath}/Makefile
sed -i "s/{{ COMPONENT_NAME }}/$COMPONENT_NAME/g" ${OutputPath}/mod.mk
echo "Don't forget to add the component to fprime/mk/configs/modules/modules.mk under CubeRover_MODULES"
