#include <Elementary.h>
#include "Elm_Utils.h"
#include "Bks_System.h"
#include "Bks_Ui.h"
#include "Bks_Ui_Private.h"

void products_page_clear(void)
{
   elm_gengrid_clear(ui.products.grid);
   elm_list_clear(ui.products.list);
}

void _product_selected_set(const Elm_Object_Item *product)
{
   if (!product) return;

   ui.products.selected = product;
   user_accounts_page_reset();
   elm_naviframe_item_promote(ui.user_accounts.enp.eoi);
}

/**
 * @brief This function resets the product selection. It does not reload the
 * data.
 */
void
products_page_reset(void)
{
   products_grid_reset();
   products_list_reset();
}

Evas_Object *_products_favs_add(void)
{
   return products_grid_add();
}

Evas_Object *_products_alpha_add(void)
{
   return products_list_add();
}

Evas_Object *products_page_add(void)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(ui.win, NULL);

   ui.products.panes = elm_panes_add(ui.win);
   elm_panes_horizontal_set(ui.products.panes, EINA_TRUE);

   ui.products.favs = _products_favs_add();
   evas_object_show(ui.products.favs);
   elm_object_part_content_set(ui.products.panes, "left", ui.products.favs);

   ui.products.alpha = _products_alpha_add();
   evas_object_show(ui.products.alpha);
   elm_object_part_content_set(ui.products.panes, "right", ui.products.alpha);

   ui.products.lock_window.win = elm_win_inwin_add(ui.win);
   ui.products.lock_window.content = elm_label_add(ui.products.lock_window.win);
   elm_object_text_set(ui.products.lock_window.content, "Die Produktliste wird aktualisiert");
   elm_win_inwin_content_set(ui.products.lock_window.win, ui.products.lock_window.content);

   return ui.products.panes;
}

void _favs_set(Eina_List *products)
{
   products_grid_set(products);
}

void _alpha_set(Eina_List *products)
{
   products_list_set(products);
}

void _async_favs_set(void *data, Ecore_Thread *th)
{
   Eina_List *products = (Eina_List*)data;

   bks_ui_products_lock_take();
   _favs_set(products);
   eina_lock_release(&ui.products.lock);
   bks_ui_products_update_set(EINA_FALSE);
}

void _async_alpha_set(void *data, Ecore_Thread *th)
{
   Eina_List *products = (Eina_List*)data;

   bks_ui_products_lock_take();
   _alpha_set(products);
   eina_lock_release(&ui.products.lock);
   bks_ui_products_update_set(EINA_FALSE);
}

const Bks_Model_Product *bks_ui_product_selected_get(void)
{
   const Bks_Model_Product *product;

   product = (Bks_Model_Product*)elm_object_item_data_get(ui.products.selected);

   return product;
}

//API calls

/**
 * @brief Clears the products ui elements.
 */
void bks_ui_products_clear(void)
{
   bks_ui_products_lock_take();
   products_page_clear();
   eina_lock_release(&ui.products.lock);
}

/**
 * @brief Indicates that the products are being refetched.
 */
void bks_ui_products_update_set(const Eina_Bool update)
{
   printf("Jetzt sollte der Produktbildschirm %sbenutzbar sein.\n", (update ? "un" : ""));

   if (!update)
     {
        evas_object_hide(ui.products.lock_window.win);
        return;
     }

   if (!evas_object_visible_get(ui.lock_window.win))
     elm_win_inwin_activate(ui.products.lock_window.win);
}

/**
 * @brief Aquires a lock for the products ui elements
 */
void bks_ui_products_lock_take(void)
{
    eina_lock_take(&ui.products.lock);
}

/**
 * @brief fills the UI's favourite's section using @p products.
 *
 * @param products list of Bks_Model_Product
 */
void bks_ui_products_page_favs_set(Eina_List *products)
{
   ecore_thread_run(_async_favs_set, NULL, NULL, products);
}

/**
 * @brief fills the UI's favourite's section using @p products.
 *
 * @param products list of Bks_Model_Product
 */
void bks_ui_products_page_alpha_set(Eina_List *products)
{
   //CREATE THREAD
   ecore_thread_run(_async_alpha_set, NULL, NULL, products);
}
