function calc(decking, laga, corner, clip, length, width) {
  var perimeter, yardage, cutting;

  cutting = 0.05;
  perimeter = 2 * (length + width);
  yardage = length * width;
  
  corner.number = perimeter * (1 + cutting);
  corner.cost = corner.number * corner.price;

  laga.number = yardage * (1 + cutting) / 0.3;
  laga.cost = laga.number * laga.price;

  clip.number = yardage * (1 + cutting) * 20;
  clip.cost = clip.number * clip.price;

  decking.number = yardage * (1 + cutting);
  decking.cost = decking.number * decking.price;

  totalCost = corner.cost + laga.cost + clip.cost + decking.cost;
    
  return {
      decking: decking,
      laga: laga,
      corner: corner,
      clip: clip,
      total: totalCost
  };
}

function dataBinding ($, context) {
  var decking = {
        lite: {price: 360},
        clasic: {price: 470},
        prof: {price: 600}
      },
      res = calc(
        decking[$("select.decking", context).val()],
        {price:32},
        {price:26},
        {price:4.4},
        Number($("input.length", context).val()),
        Number($("input.width", context).val())
      ),
      bind = ['decking', 'laga', 'corner', 'clip'],
      i;

  for(i=0; i<bind.length; i++) {
    $('.' + bind[i] + ' ' + '.number', context).text('Количество: ' + (res[bind[i]]["number"]).toFixed(2));
    $('.' + bind[i] + ' ' + '.cost', context).text('Стоимость: ' + (res[bind[i]]["cost"]).toFixed(2));
  }
  $('.total .cost', context).text('cтоимость: ' + (res['total']).toFixed(2));
}

function isNumeric(n) {
  return !isNaN(parseFloat(n)) && isFinite(n);
}

(function ($) {
  $(function () {

    var $calcBlock = $('#calc_block');
    
    $('input, textarea').placeholder();
    
    if ($calcBlock.length > 0) {
      $calcBlock.dialog({
        autoOpen: false,
        title: 'Калькулятор заказа',
        resizable: false,
        modal: true
      });
    }

    $('#calc_btn').click(function () {
      $calcBlock.dialog('open');

      dataBinding($, $calcBlock);

      $("select.decking", $calcBlock).change(function () {
        dataBinding($, $calcBlock);
      });

      $("input.length", $calcBlock).change(function () {
        if(isNumeric($(this).val())) {
          dataBinding($, $calcBlock);
          $(this).removeClass("input_error");
        } else {
          $(this).addClass("input_error");
          $(this).val("");
          $(this).attr("placeholder", "Введите число");
        }
      });

      $("input.width", $calcBlock).change(function () {
        if(isNumeric($(this).val())) {
          dataBinding($, $calcBlock);
          $(this).removeClass("input_error");
        } else {
          $(this).addClass("input_error");
          $(this).val("");
          $(this).attr("placeholder", "Введите число");
        }
      });

      return false;
    });
  });
}(jQuery));