#include "Bks_Types.h"
#include <Evas.h>
#include <Elementary.h>
#include "Bks_System.h"
#include "Bks_Ui.h"


void _bks_ui_sale_notification_set(Bks_Model_Sale *sale) {

   Eina_List *l, *accs, *prods;
   int accs_count, prods_count;
   int size, length;
   char *text;
   Bks_Model_User_Account *acc;
   Bks_Model_Product *prod;
   float sum = 0;

   if (sale->status == BKS_MODEL_SALE_DONE) {

      accs = sale->user_accounts;
      accs_count = eina_list_count(accs);
      prods = sale->products;
      prods_count = eina_list_count(prods);
      // size for text + estimated size for user accounts
      size = 120 + accs_count * 30 + prods_count * 30;
      text = calloc((size_t)(size + 1),1);
      length = snprintf(text, (size_t)size, "<align = left>Kauf von ");

      EINA_LIST_FOREACH(prods, l, prod) {
         if (size > length) {
            length += snprintf(text + length,(size_t)(size - length), "%s: %.2f EUR<br>", prod->name, prod->price);
         }
         sum +=prod->price;
      }


      length += snprintf(text + length, (size_t)(size - length), "<b>Summe: %.2f EUR</b><br>wurde ", sum);
      // append every name in list
      EINA_LIST_FOREACH(accs, l, acc) {
         if (size > length) {
            length += snprintf(text + length,(size_t)(size - length), "%s %s, ", acc->firstname, acc->lastname);
         }
      }
      if (size - length > 20) { // space for more than 20 char available
         length += snprintf(text + length - 2, (size_t)((size -length) - 2), " berechnet.</align>");
      } else { // not enough space, truncate names
         snprintf(text + size - 23, 23,"... berechnet.</align>");
      }

      elm_object_text_set(ui.notification.note, text);
      evas_object_show(ui.notification.note);
      elm_popup_timeout_set(ui.notification.note, 2.0 + 2*(double)accs_count + 2*(double)prods_count);
      free(text);
   } else  {
      elm_object_text_set(ui.notification.note, "ERROR: Verkauf konnte nicht durchgeführt werden!");
      evas_object_show(ui.notification.note);
      elm_popup_orient_set(ui.notification.note, ELM_POPUP_ORIENT_CENTER);
      elm_popup_allow_events_set(ui.notification.note, EINA_FALSE);
      elm_popup_timeout_set(ui.notification.note, -1.0);
   }
}
void _bks_ui_sale_notification_clicked(void *data UNUSED, Evas_Object *obj, void *event_info UNUSED) {

   evas_object_hide(obj);
   elm_popup_orient_set(ui.notification.note, ELM_POPUP_ORIENT_BOTTOM_RIGHT);
   elm_popup_allow_events_set(ui.notification.note, EINA_TRUE);
}
