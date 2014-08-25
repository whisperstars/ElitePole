(function ($) {
	$(document).ready(function () {
		$(".field-collection-item-field-photo-collection-test").hide();
		$(".field-type-entityreference .field-items a").click(function (e) {
			e.preventDefault();
			var href = $(this).attr("href");
			
			/*if(!sl[href]) {
				sl[href] = true;
				$(".node-content").load(href + " .field-collection-item-field-photo-collection-test");
			}*/

			$(".field-collection-item-field-photo-collection-test").hide();
			$("div[about='"+ href +"']").show();
		});
	});
}(jQuery));