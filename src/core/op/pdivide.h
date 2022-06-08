#ifndef pdivide_H
#define pdivide_H

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * t_pdivide: Frequency divider for a pulse train
     *
     * bool - pulse signal to divide
     * int - divisor (must be 1 or greater)
     */

    typedef struct
    {
        bool lastPulse;
        int count;
    } t_pdivide;

    void pdivide_init(t_pdivide *self);
    float pdivide_process(t_pdivide *self, bool signal, int divisor);

#ifdef __cplusplus
}
#endif

#endif // pdivide_H
