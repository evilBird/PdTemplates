//
//  template.c
//  PdExtenalTemplate
//
//  Created by Travis Henspeter on 11/18/15.
//  Copyright © 2015 birdSound. All rights reserved.
//
//  An template (non-DSP) Pd external.

#include <stdio.h>
#include "m_pd.h"


static t_class *template_class;

//The object layout

typedef struct _template {
    t_object  x_obj;
    t_int i_count;
    t_float step;
    t_int i_down, i_up;
    t_outlet *f_out, *b_out;
} t_template;

//Called when inlet receives a bang

void template_bang(t_template *x)
{
    t_float f=x->i_count;
    t_int step = x->step;
    x->i_count+=step;
    
    if (x->i_down-x->i_up) {
        if ((step>0) && (x->i_count > x->i_up)) {
            x->i_count = x->i_down;
            outlet_bang(x->b_out);
        } else if (x->i_count < x->i_down) {
            x->i_count = x->i_up;
            outlet_bang(x->b_out);
        }
    }
    
    outlet_float(x->f_out, f);
}

//Called when inlet receives list with 'reset' as its first arg


void template_reset(t_template *x)
{
    x->i_count = x->i_down;
}

//Called when inlet receives list with 'set' as its first arg


void template_set(t_template *x, t_floatarg f)
{
    x->i_count = f;
}

//Called when inlet receives list with 'bound' as its first arg

void template_bound(t_template *x, t_floatarg f1, t_floatarg f2)
{
    x->i_down = (f1<f2)?f1:f2;
    x->i_up   = (f1>f2)?f1:f2;
}

//instantiates an instance of the object class

void *template_new(t_symbol *s, int argc, t_atom *argv)
{
    t_template *x = (t_template *)pd_new(template_class);
    t_float f1=0, f2=0;
    
    x->step=1;
    switch(argc){
        default:
        case 3:
            x->step=atom_getfloat(argv+2);
        case 2:
            f2=atom_getfloat(argv+1);
        case 1:
            f1=atom_getfloat(argv);
            break;
        case 0:
            break;
    }
    if (argc<2)f2=f1;
    
    x->i_down = (f1<f2)?f1:f2;
    x->i_up   = (f1>f2)?f1:f2;
    
    x->i_count=x->i_down;
    
    inlet_new(&x->x_obj, &x->x_obj.ob_pd,
              gensym("list"), gensym("bound"));
    floatinlet_new(&x->x_obj, &x->step);
    
    x->f_out = outlet_new(&x->x_obj, &s_float);
    x->b_out = outlet_new(&x->x_obj, &s_bang);
    
    return (void *)x;
}

//Creates the object class, defines class methods help symbol

void template_setup(void) {
    template_class = class_new(gensym("template"),
                              (t_newmethod)template_new,
                              0, sizeof(t_template),
                              CLASS_DEFAULT,
                              A_GIMME, 0);
    
    class_addbang  (template_class, template_bang);
    class_addmethod(template_class,
                    (t_method)template_reset, gensym("reset"), 0);
    class_addmethod(template_class,
                    (t_method)template_set, gensym("set"),  
                    A_DEFFLOAT, 0);  
    class_addmethod(template_class,  
                    (t_method)template_bound, gensym("bound"),  
                    A_DEFFLOAT, A_DEFFLOAT, 0);  
    
    class_sethelpsymbol(template_class, gensym("help-template"));  
}