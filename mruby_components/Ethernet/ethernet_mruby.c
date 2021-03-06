#include <core.h>
#include <stdint.h>
#include <core/printf.h>
#include <core/thread.h>
#include <core/time.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/irep.h>
#include <mruby/proc.h>
uint8_t mrb_ethernet_code[];
void
*allocate(struct mrb_state *mrb, void *p, size_t size, void *ud)
{
    if (size == 0){
        if(p != 0 ){
            free(p);
        }
        return NULL;
    }else {
        return realloc(p, size);
    }
}
mrb_value
bitvisor_print(mrb_state *mrb,mrb_value self)
{
    mrb_value str;
    mrb_get_args(mrb, "S", &str);
    printf("%s", RSTRING_PTR(str));
}

mrb_value
bitvisor_get_time(mrb_state *mrb,mrb_value self)
{
    mrb_int a;
    a = get_time();
    return mrb_fixnum_value(a);
}
mrb_value
bitvisor_set_schedule(mrb_state *mrb,mrb_value self)
{
    schedule();
}
mrb_value bitvisor_sendnic(mrb_state *mrb,mrb_value self);

mrb_state *mrb;

static void
ethernet_thread(void *arg)
{
    mrb = mrb_open_allocf(allocate,NULL);
    printf("mruby opend\n");
    struct RClass *bitvisor;
    if(mrb != NULL){
        bitvisor = mrb_define_class(mrb,"Bitvisor",mrb->object_class);
        mrb_define_class_method(mrb,bitvisor,"print",bitvisor_print,MRB_ARGS_REQ(1));
        mrb_define_class_method(mrb,bitvisor,"get_time",bitvisor_get_time,MRB_ARGS_NONE());
        mrb_define_class_method(mrb,bitvisor,"get_dest_macaddr",bitvisor_sendnic,MRB_ARGS_NONE());
        mrb_define_class_method(mrb,bitvisor,"set_schedule",bitvisor_set_schedule,MRB_ARGS_NONE());
        mrb_load_irep(mrb,mrb_ethernet_code);

        mrbc_context *cxt = mrbc_context_new(mrb);
        mrb_load_string_cxt(mrb,"",cxt);
        mrbc_context_free(mrb,cxt);
        mrb_close(mrb);
    }
    thread_exit();
}

static void
ethernet_kernel_init(void)
{
    printf("ethernet_kernel_init invoked.\n");
    thread_new(ethernet_thread, NULL, VMM_STACKSIZE);
}
INITFUNC("vmmcal0", ethernet_kernel_init);
