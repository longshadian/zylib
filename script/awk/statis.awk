
######统计脚本
######

BEGIN {
    total_chong_zhi = 0
    total_fen_xiang = 0
    total_xiao_hao = 0
}

##统计充值
$4 == "statistic_http_charge" {
    uid = $6
    fk = $8
    total_chong_zhi += fk
	#print $8
}

##统计扣房卡
$4 == "statistic_kou_fang_ka" {
    total_xiao_hao += $8
	#print $0
}

##统计分享
$4 == "statistic_fen_xiang" {
    fk = $8
    total_fen_xiang += fk
	#print $0
}

##统计注册送房卡
#$4 == "statistic_register" {
#    print $0
#}


END {
    print "充值 " total_chong_zhi
    print "分享 " total_fen_xiang
    print "消耗 " total_xiao_hao
}
