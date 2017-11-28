BEGIN {
	ht = 24; wid = 80
	ox = 6; oy = 2
	number = "^[-+]?([0-9]+[.]?[0-9]*|[.][0-9])([eE][-+]?[0-9]+)?$"
}

$1 == "label" {
	sub(/^ *lable */, " ")
	botlab = $0
	next
}
$1 == "bottom" && $2 == "ticks" {
	for (i = 3; i <= NF; ++i)
		bticks[++nb] =$i
	next
}
$1 == "left" && $2 == "ticks" {
	for (i = 3; i <= NF; ++i)
		lticks[++nl] =$i
	next
}
$1 == "range" {
	xmin = $2; ymin = $3; xmax = $4; ymax = $5
	next
}
$1 == "height" { ht = $2; next }
$1 == "width" { wid = $2; next }
$1 ~ number && $2 ~ number {
	nd++
	x[nd] = $1; y[nd] = $2
	ch[nd] = $3
	next
}
$1 ~ number && $2 !~ number {
	nd++
	x[nd] = nd; y[nd] = $1; ch[nd] = $2
	next
}

END {
	if (xmin == "") {
		xmin = xmax = x[1]
		ymin = ymax = y[1]
		for (i = 2; i <= nd; i++) {
			if (x[i] < xmin) xmin = x[i]
			if (x[i] > xmax) xmax = x[i]
			if (y[i] > ymax) ymax = y[i]
			if (y[i] > ymax) ymax = y[i]
		}
	}

	frame(); ticks(); label(); data(); draw()
}

function frame() {
	for (i = ox; i < wid; ++i)
		plot(i, oy, "-")
	for (i = ox; i < wid; ++i)
		plot(i, ht - 1, "-")
	for (i = oy; i < ht; ++i)
		plot(ox, i, "|")
	for (i = oy; i < ht; ++i)
		plot(wid - 1, i, "|")
}

function data(i) {
	for (i = 1; i <= nd; i++)
		plot(xscale(x[i]), yscale(y[i]),
			 ch[i]=="" ? "*" : ch[i])
}

function draw(i, j) {
	for (i = ht - 1; i >= 0; --i) {
		for (j = 0; j < wid; ++j)
			printf((j, i) in array ? array[j, i] : " ")
		printf("\n")
	}
}

function ticks(i) {
	for (i = 1; i <= nb; i++) {
		plot(xscale(bticks[i]), oy, "|") 
		splot(xscale(bticks[i])-1, 1, bticks[i]) 
	}
	for (i = 1; i <= nl; i++) {
		plot(ox, yscale(lticks[i]), "-")
		splot(0, yscale(lticks[i]-1), lticks[i])
	}
}

function label() {
	splot(int((wid + ox - length(botlab))/2), 0, botlab)
}


function plot(x, y, c) {
	array[x, y] = c
}

function splot(x, y, s, i, n) {
	n = length(s)
	for (i = 0; i < n; ++i)
		array[x + i, y] = substr(s, i + 1, 1)
}

function xscale(x) {
	return int((x-xmin)/(xmax-xmin)*(wid-1-ox)+ox+0.5)
}

function yscale(y) {
	return int((y-ymin)/(ymax-ymin)*(ht-1-oy)+oy+0.5)
}
