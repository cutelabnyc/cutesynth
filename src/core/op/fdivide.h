#ifndef fdivide_H
#define fdivide_H

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * fdivide_t: Frequency divider for a phasor signal
     *
     * float - phasor signal to divide
     * int - divisor
     */

    typedef struct
    {
        float lastPhase;
        int count;
    } t_fdivide;

    void fdivide_init(t_fdivide *self);
    float fdivide_process(t_fdivide *self, float signal, int divisor);

#ifdef __cplusplus
}
#endif

#endif // fdivide_H
