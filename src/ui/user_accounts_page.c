#include <Evas.h>
#include "Elm_Utils.h"
#include "Bks_System.h"
#include "Bks_Ui.h"
#include "Bks_Ui_Private.h"

void user_accounts_page_clear(void)
{
   elm_list_clear(ui.user_accounts.list);
}

static void
_on_user_accounts_prev_btn_click(void *data, Evas_Object *obj, void *event_info)
{
   products_page_reset();
   elm_naviframe_item_promote(ui.products.enp.eoi);
}

static void
_on_user_accounts_finish_btn_click(void *data, Evas_Object *obj, void *event_info)
{
   bks_controller_ui_sale_finish_cb();
   elm_list_selection_clear(ui.products.list);
   elm_naviframe_item_promote(ui.products.enp.eoi);
}

void
user_accounts_page_reset(void)
{
   elm_list_selection_clear(ui.user_accounts.list);
   elm_object_disabled_set(ui.user_accounts.enp.next_btn, EINA_TRUE);
}

   Evas_Object
*user_accounts_page_add(void)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(ui.win, NULL);

   //create, setup and fill user_accounts
   ui.user_accounts.list = user_accounts_page_list_add();
   // Add button to go back to productslist
   ui.user_accounts.enp.prev_btn = elm_button_add(ui.naviframe);
   evas_object_show(ui.user_accounts.enp.prev_btn);
   elm_object_text_set(ui.user_accounts.enp.prev_btn, "Zurück");
   evas_object_smart_callback_add(ui.user_accounts.enp.prev_btn, "clicked", _on_user_accounts_prev_btn_click, NULL);
   // Add button to finish shopping
   ui.user_accounts.enp.next_btn = elm_button_add(ui.naviframe);
   evas_object_show(ui.user_accounts.enp.next_btn);
   elm_object_text_set(ui.user_accounts.enp.next_btn, "Fertig");
   evas_object_smart_callback_add(ui.user_accounts.enp.next_btn, "clicked", _on_user_accounts_finish_btn_click, NULL);

   ui.user_accounts.lock_window.win = elm_win_inwin_add(ui.win);
   ui.user_accounts.lock_window.content = elm_label_add(ui.user_accounts.lock_window.win);
   elm_object_text_set(ui.user_accounts.lock_window.content, "Die Benutzerkontenliste wird aktualisiert");
   elm_win_inwin_content_set(ui.user_accounts.lock_window.win, ui.user_accounts.lock_window.content);

   return ui.user_accounts.list;
}

void user_accounts_page_set(Eina_List *user_accounts)
{
   user_accounts_list_set(user_accounts);
}

void _async_page_set(void *data, Ecore_Thread *th)
{
   Eina_List *user_accounts = (Eina_List*)data;

   bks_ui_user_accounts_lock_take();
   user_accounts_page_set(user_accounts);
   eina_lock_release(&ui.user_accounts.lock);
   bks_ui_user_accounts_update_set(EINA_FALSE);
}

// API calls

/**
 * @brief Clears the user accounts ui elements.
 */
void bks_ui_user_accounts_clear(void)
{
   bks_ui_user_accounts_lock_take();
   user_accounts_page_clear();
   eina_lock_release(&ui.user_accounts.lock);
}

/**
 * @brief Indicates that the user accounts are being refetched.
 */
void bks_ui_user_accounts_update_set(const Eina_Bool update)
{
   printf("Jetzt sollte der Benutzerlistenschirm %sbenutzbar sein.\n", (update ? "un" : ""));
   if (!update)
     {
        evas_object_hide(ui.user_accounts.lock_window.win);
        return;
     }

   if (!evas_object_visible_get(ui.lock_window.win))
     elm_win_inwin_activate(ui.user_accounts.lock_window.win);
}

void bks_ui_user_accounts_page_set(Eina_List *user_accounts)
{
   ecore_thread_run(_async_page_set, NULL, NULL, user_accounts);
}

/**
 * @return List of Bks_Model_User_Account elements
 */
Eina_List *bks_ui_user_accounts_selected_get(void)
{
   const Eina_List *selected_accounts;
   Eina_List *iter, *list = NULL;
   Elm_Object_Item *eoi;
   const Bks_Model_User_Account *acc;

   if (!(selected_accounts = elm_list_selected_items_get(ui.user_accounts.list)))
       return NULL;

   // print the names of all selected accounts
   EINA_LIST_FOREACH((Eina_List*)selected_accounts, iter, eoi)
     {
        if (elm_list_item_selected_get(eoi))
          {
             acc = (Bks_Model_User_Account*)elm_object_item_data_get(eoi);
             printf("Selected User Account: %s, %s\n", acc->lastname, acc->firstname);
             list = eina_list_append(list, acc);
          }
     }

   return list;
}

/**
 * @brief Aquires a lock for the user accounts ui elements
 */
void bks_ui_user_accounts_lock_take(void)
{
    eina_lock_take(&ui.user_accounts.lock);
}
