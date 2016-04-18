var cvs = document.getElementById("canvas"),
ctx = cvs.getContext("2d"),
touchRadius = 30,  
winWidth = window.innerWidth,
winHeight = window.innerHeight;


cvs.width = winWidth;
cvs.height = winHeight;
ctx.fillStyle = "#fff";
ctx.fillRect(0, 0, winWidth, winHeight);    

var fillCircle = function (x, y, radius, fillColor) {
	this.fillStyle = fillColor || '#fff';
	this.beginPath();
	this.moveTo(x, y);
	this.arc(x, y, radius, 0, Math.PI * 2);   
	this.fill();
};


var device = /android|webos|iphone|ipad|ipod|blackberry|iemobile|opera mini/i.test(navigator.userAgent.toLowerCase()),
clickEvtName = device ? 'touchstart' : 'mousedown',
moveEvtName = device ? 'touchmove' : 'mousemove';


if (!device) {
	var isMouseDown = false;
	document.addEventListener('mouseup', function () {
		isMouseDown = false;
	}, false);
} else {
	document.addEventListener("touchmove", function (e) {
		if (isMouseDown) {
			e.preventDefault();
		}
	}, false);
	document.addEventListener('touchend', function (e) {
		isMouseDown = false;
	}, false);
}


cvs.addEventListener(clickEvtName, function (e) {
	isMouseDown = true;
	var x = (device ? e.touches[0].clientX : e.clientX);
	var y = (device ? e.touches[0].clientY : e.clientY);
	ctx.globalCompositeOperation = 'destination-out';
	fillCircle.call(ctx, x, y-55, touchRadius);
}, false);


cvs.addEventListener(moveEvtName, function (e) {
	if (!device && !isMouseDown) {
		return false;
	}
	var x = (device ? e.touches[0].clientX : e.clientX);
	var y = (device ? e.touches[0].clientY : e.clientY);
	ctx.globalCompositeOperation = 'destination-out';
	fillCircle.call(ctx, x, y-55, touchRadius);
}, false);
