mserver5 --set monet_prompt='' <fastcrack_select_void_low_hgh.mal |tail -1 
mserver5 --set monet_prompt='' <fastcrack_select_void_mid.mal |tail -1 
mserver5 --set monet_prompt='' <fastcrack_select_low_hgh.mal |tail -1
mserver5 --set monet_prompt='' <fastcrack_select_mid.mal |tail -1
mserver5 --set monet_prompt='' <fastcrack_scan.mal |tail -1
mserver5 --set monet_prompt='' <fastcrack_copy.mal |tail -1
mserver5 --set monet_prompt='' <fastcrack_sel_EQ_mid.mal |tail -1
mserver5 --set monet_prompt='' <fastcrack_sel_LE_mid.mal |tail -1
mserver5 --set monet_prompt='' <fastcrack_sel_low_hgh.mal |tail -1
mserver5 --set monet_prompt='' <fastcrack_sel_ALL.mal |tail -1
mserver5 --set monet_prompt='' <fastcrack_sort.mal |tail -1

echo 'tail<=MID , tail>MID'
mserver5 --set monet_prompt='' <fastcrack_Zero_MK.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
#mserver5 --set monet_prompt='' <fastcrack_Zero_SI.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_Zero_ms.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_Zero_SM.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
echo 'tail<MID , tail==MID , tail>MID'
mserver5 --set monet_prompt='' <fastcrack_One_MK.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_One_SI.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_One_ms.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_One_SM.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
echo 'tail<=LOW , LOW<tail<=HGH , tail>HGH'
mserver5 --set monet_prompt='' <fastcrack_Three_MK.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_Three_SI.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_Three_ms.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_Three_SM.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
echo 'tail<LOW , tail==LOW , LOW<tail<HGH , tail==HGH , tail>HGH'
mserver5 --set monet_prompt='' <fastcrack_Two_MK.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
#mserver5 --set monet_prompt='' <fastcrack_Two_SI.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_Two_ms.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
mserver5 --set monet_prompt='' <fastcrack_Two_SM.mal |egrep -v '^[# ]|, true\]$|^\[ "" \]$'
