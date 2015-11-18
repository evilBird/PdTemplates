//
//  template~.c
//  template~
//
//  Created by Travis Henspeter on 11/18/15.
//  Copyright © 2015 birdSound. All rights reserved.
//
//  Template for DSP Pd External. Signal panner.

#include <stdio.h>
#include "m_pd.h"

static t_class *template_tilde_class;

typedef struct _template_tilde {
    t_object  x_obj;
    t_sample f_pan;
    t_sample f;
    
    t_inlet *x_in2;
    t_inlet *x_in3;
    t_outlet*x_out;
} t_template_tilde;

// Perform routine is main DSP processing

t_int *template_tilde_perform(t_int *w)
{
    t_template_tilde *x = (t_template_tilde *)(w[1]);
    t_sample  *in1 =    (t_sample *)(w[2]);
    t_sample  *in2 =    (t_sample *)(w[3]);
    t_sample  *out =    (t_sample *)(w[4]);
    int          n =           (int)(w[5]);
    t_sample f_pan = (x->f_pan<0)?0.0:(x->f_pan>1)?1.0:x->f_pan;
    
    while (n--) *out++ = (*in1++)*(1-f_pan)+(*in2++)*f_pan;
    
    return (w+6);
}

//Declare dsp methods to be added to the processing graph

void template_tilde_dsp(t_template_tilde *x, t_signal **sp)
{
    dsp_add(template_tilde_perform, 5, x,
            sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

// Destructor

void template_tilde_free(t_template_tilde *x)
{
    inlet_free(x->x_in2);
    inlet_free(x->x_in3);
    outlet_free(x->x_out);
}

// Instantiates an instance of the class

void *template_tilde_new(t_floatarg f)
{
    t_template_tilde *x = (t_template_tilde *)pd_new(template_tilde_class);
    
    x->f_pan = f;
    
    x->x_in2=inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    x->x_in3=floatinlet_new (&x->x_obj, &x->f_pan);
    x->x_out=outlet_new(&x->x_obj, &s_signal);
    
    return (void *)x;
}

// Creates the class and defines its methods

void template_tilde_setup(void) {
    template_tilde_class = class_new(gensym("template~"),
                                (t_newmethod)template_tilde_new,
                                0, sizeof(t_template_tilde),
                                CLASS_DEFAULT,
                                A_DEFFLOAT, 0);
    
    class_addmethod(template_tilde_class,  
                    (t_method)template_tilde_dsp, gensym("dsp"), 0);  
    CLASS_MAINSIGNALIN(template_tilde_class, t_template_tilde, f);  
}