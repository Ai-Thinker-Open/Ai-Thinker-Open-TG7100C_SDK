/**
 * @file lv_lmeter.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_LMETER != 0

#include "lv_lmeter.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_group.h"
#include "../lv_misc/lv_trigo.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_lmeter_design(lv_obj_t * lmeter, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_lmeter_signal(lv_obj_t * lmeter, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a line meter objects
 * @param par pointer to an object, it will be the parent of the new line meter
 * @param copy pointer to a line meter object, if not NULL then the new object will be copied from it
 * @return pointer to the created line meter
 */
lv_obj_t * lv_lmeter_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of line meter*/
    lv_obj_t * new_lmeter = lv_obj_create(par, copy);
    lv_mem_assert(new_lmeter);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_lmeter);
    
    /*Allocate the line meter type specific extended data*/
    lv_lmeter_ext_t * ext = lv_obj_allocate_ext_attr(new_lmeter, sizeof(lv_lmeter_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->min_value = 0;
    ext->max_value = 100;
    ext->cur_value = 0;
    ext->line_cnt = 31;    /*Odd scale number looks better*/
    ext->scale_angle = 240; /*(scale_num - 1) * N looks better */

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_lmeter, lv_lmeter_signal);
    lv_obj_set_design_func(new_lmeter, lv_lmeter_design);

    /*Init the new line meter line meter*/
    if(copy == NULL) {
        lv_obj_set_size(new_lmeter, LV_DPI, LV_DPI);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_lmeter_set_style(new_lmeter, th->lmeter);
        } else {
            lv_lmeter_set_style(new_lmeter, &lv_style_pretty_color);
        }
    }
    /*Copy an existing line meter*/
    else {
    	lv_lmeter_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->scale_angle = copy_ext->scale_angle;
        ext->line_cnt = copy_ext->line_cnt;
        ext->min_value = copy_ext->min_value;
        ext->max_value = copy_ext->max_value;
        ext->cur_value = copy_ext->cur_value;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_lmeter);
    }
    
    return new_lmeter;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the line meter
 * @param lmeter pointer to a line meter object
 * @param value new value
 */
void lv_lmeter_set_value(lv_obj_t *lmeter, int16_t value)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    ext->cur_value = value > ext->max_value ? ext->max_value : value;
    ext->cur_value = ext->cur_value < ext->min_value ? ext->min_value : ext->cur_value;
    lv_obj_invalidate(lmeter);
}

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void lv_lmeter_set_range(lv_obj_t *lmeter, int16_t min, int16_t max)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    ext->max_value = max;
    ext->min_value = min;
    if(ext->cur_value > max) {
        ext->cur_value = max;
        lv_lmeter_set_value(lmeter, ext->cur_value);
    }
    if(ext->cur_value < min) {
        ext->cur_value = min;
        lv_lmeter_set_value(lmeter, ext->cur_value);
    }
    lv_obj_invalidate(lmeter);
}

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param line_cnt number of lines
 */
void lv_lmeter_set_scale(lv_obj_t * lmeter, uint16_t angle, uint8_t line_cnt)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    ext->scale_angle = angle;
    ext->line_cnt = line_cnt;

    lv_obj_invalidate(lmeter);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the value of the line meter
 */
int16_t lv_lmeter_get_value(lv_obj_t *lmeter)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->cur_value;
}

/**
 * Get the minimum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the minimum value of the line meter
 */
int16_t lv_lmeter_get_min_value(lv_obj_t * lmeter)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->min_value;
}

/**
 * Get the maximum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the maximum value of the line meter
 */
int16_t lv_lmeter_get_max_value(lv_obj_t * lmeter)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->max_value;
}

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint8_t lv_lmeter_get_line_count(lv_obj_t * lmeter)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->line_cnt ;
}

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle of the scale
 */
uint16_t lv_lmeter_get_scale_angle(lv_obj_t * lmeter)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->scale_angle;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the line meters
 * @param lmeter pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_lmeter_design(lv_obj_t * lmeter, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_lmeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
        lv_style_t * style = lv_obj_get_style(lmeter);

        lv_style_t style_tmp;
        memcpy(&style_tmp, style, sizeof(lv_style_t));


#if USE_LV_GROUP
        lv_group_t *g = lv_obj_get_group(lmeter);
        if(lv_group_get_focused(g) == lmeter) {
            style_tmp.line.width += 1;
        }
#endif

         lv_coord_t r_out = lv_obj_get_width(lmeter) / 2;
         lv_coord_t r_in = r_out - style->body.padding.hor;
         lv_coord_t x_ofs = lv_obj_get_width(lmeter) / 2 + lmeter->coords.x1;
         lv_coord_t y_ofs = lv_obj_get_height(lmeter) / 2 + lmeter->coords.y1;
         int16_t angle_ofs = 90 + (360 - ext->scale_angle) / 2;
         int16_t level = (int32_t)((int32_t)(ext->cur_value - ext->min_value) * ext->line_cnt) / (ext->max_value - ext->min_value);
         uint8_t i;

         style_tmp.line.color = style->body.main_color;

         for(i = 0; i < ext->line_cnt; i++) {
             /*Calculate the position a scale label*/
             int16_t angle = (i * ext->scale_angle) / (ext->line_cnt - 1) + angle_ofs;

             lv_coord_t y_out = (int32_t)((int32_t)lv_trigo_sin(angle) * r_out) / TRIGO_SIN_MAX;
             lv_coord_t x_out = (int32_t)((int32_t)lv_trigo_sin(angle + 90) * r_out) / TRIGO_SIN_MAX;
             lv_coord_t y_in = (int32_t)((int32_t)lv_trigo_sin(angle) * r_in) / TRIGO_SIN_MAX;
             lv_coord_t x_in = (int32_t)((int32_t)lv_trigo_sin(angle + 90) * r_in) / TRIGO_SIN_MAX;

             lv_point_t p1;
             lv_point_t p2;

             p2.x = x_in + x_ofs;
             p2.y = y_in +  y_ofs;

             p1.x = x_out+ x_ofs;
             p1.y = y_out + y_ofs;

             if(i >= level) style_tmp.line.color = style->line.color;
             else {
                 style_tmp.line.color = lv_color_mix(style->body.grad_color, style->body.main_color, (255 * i) /  ext->line_cnt);
             }

             lv_draw_line(&p1, &p2, mask, &style_tmp);
         }

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Signal function of the line meter
 * @param lmeter pointer to a line meter object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_lmeter_signal(lv_obj_t * lmeter, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(lmeter, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}


#endif
