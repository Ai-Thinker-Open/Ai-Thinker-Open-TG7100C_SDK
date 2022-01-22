/**
 * @file lv_templ.c
 * 
 */

/* TODO Remove these instructions
 * Search an replace: template -> object normal name with lower case (e.g. button, label etc.)
 *                    templ -> object short name with lower case(e.g. btn, label etc)
 *                    TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_TEMPL != 0

#include "lv_templ.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_templ_design(lv_obj_t * templ, const area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_templ_signal(lv_obj_t * templ, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a template object
 * @param par pointer to an object, it will be the parent of the new template
 * @param copy pointer to a template object, if not NULL then the new object will be copied from it
 * @return pointer to the created template
 */
lv_obj_t * lv_templ_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of template*/
	/*TODO modify it to the ancestor create function */
    lv_obj_t * new_templ = lv_ANCESTOR_create(par, copy);
    dm_assert(new_templ);
    
    /*Allocate the template type specific extended data*/
    lv_templ_ext_t * ext = lv_obj_alloc_ext(new_templ, sizeof(lv_templ_ext_t));
    dm_assert(ext);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_templ);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_templ);

    /*Initialize the allocated 'ext' */
    ext->xyz = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_templ, lv_templ_signal);
    lv_obj_set_design_f(new_templ, lv_templ_design);

    /*Init the new template template*/
    if(copy == NULL) {
        lv_obj_set_style(new_templ, lv_style_get(LV_STYLE_PRETTY, NULL));
    }
    /*Copy an existing template*/
    else {
    	lv_templ_ext_t * copy_ext = lv_obj_get_ext(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_templ);
    }
    
    return new_templ;
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */


/*=====================
 * Setter functions
 *====================*/

/*
 * New object specific "set" functions come here
 */


/**
 * Set a style of a template.
 * @param templ pointer to template object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_templ_set_style(lv_obj_t * templ, lv_templ_style_t type, lv_style_t *style)
{
    lv_templ_ext_t *ext = lv_obj_get_ext_attr(templ);

    switch (type) {
        case LV_TEMPL_STYLE_X:
            break;
        case LV_TEMPL_STYLE_Y:
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

/**
 * Get style of a template.
 * @param templ pointer to template object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
lv_style_t * lv_btn_get_style(lv_obj_t * templ, lv_templ_style_t type)
{
    lv_templ_ext_t *ext = lv_obj_get_ext_attr(templ);

    switch (type) {
        case LV_TEMPL_STYLE_X:     return NULL;
        case LV_TEMPL_STYLE_Y:     return NULL;
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the templates
 * @param templ pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_templ_design(lv_obj_t * templ, const area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Signal function of the template
 * @param templ pointer to a template object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_templ_signal(lv_obj_t * templ, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_ancestor_signal(templ, sign, param);
    if(res != LV_RES_OK) return res;


    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}

#endif
