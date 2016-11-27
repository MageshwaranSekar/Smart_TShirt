var thisgroup=0;
$(document).ready(function () {
				
	$('#g0').click(function (event) {
		event.preventDefault();
		window.thisgroup = 0;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");
	});

	$('#g1').click(function (event) {
		event.preventDefault();
		window.thisgroup = 1;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");
	});

	$('#g2').click(function (event) {
		event.preventDefault();
		window.thisgroup = 2;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");
	});

	$('#g3').click(function (event) {
		event.preventDefault();
		window.thisgroup = 3;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");
	});

	$('#g4').click(function (event) {
		event.preventDefault();
		window.thisgroup = 4;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");
	});

	$('#g5').click(function (event) {
		event.preventDefault();
		window.thisgroup = 5;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");
	});

	$('#g6').click(function (event) {
		event.preventDefault();
		window.thisgroup = 6;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");
	});

	$('#g7').click(function (event) {
		event.preventDefault();
		window.thisgroup = 7;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");	
	});

	$('#g8').click(function (event) {
		event.preventDefault();
		window.thisgroup = 8;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");	
	});

	$('#g9').click(function (event) {
		event.preventDefault();
		window.thisgroup = 9;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");	
	});

	$('#g10').click(function (event) {
		event.preventDefault();
		window.thisgroup = 10;
		$( "#footer" ).replaceWith( "<h4 id=\"footer\"><em>Instantaneous values from team "+thisgroup+"</em></h4>");	
	});

	

});

		

setInterval(function () { autoloadpage(); }, 500); // it will call the function autoload() after each 1 second(s). 
function autoloadpage() {		
	$.ajax({
		url: "valeurs.html?value=instant&group="+thisgroup,
		dataType: "html",
		cache: true,
	
		success: function(data){
			$("#dispDiv").html(data);	
			
		}
	});
}

