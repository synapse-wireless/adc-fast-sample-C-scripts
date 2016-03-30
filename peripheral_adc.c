#define ENABLE_BIT_DEFINITIONS
#include <ioavr.h>
#include <iom128rfa1.h>
#include "snappy.h"

#define REFERENCE_VOLTAGE       ((1 << REFS1) | (1 << REFS0))

int16_t take_sample()
{
    // take a sample
    ADCSRA |= (1 << ADSC) | (1 << ADIF);
    while ( ADCSRA & (1<<ADSC) ) ;
    
    // return when clear
    return ADC;
}

// In this example, we are assuming we're only interested in taking
// samples from a single ended ADC (one of channels ADC0-7).
void enable_adc_and_set_channel(int16_t channel)
{
    ADCSRA &= (1 << ADEN);
    
    //ADMUX = (channel & 0x1F);
    ADMUX = REFERENCE_VOLTAGE | (channel & 0x1F);
    
    // select the ADC we want
    ADCSRB &= ~(1 << MUX5);
    
    // start one converstion to throw away to let the power supply settle
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS1);    

    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC)) ;

    // Now reset the sample rate to 4MHz
    ADCSRA = (1 << ADEN) | (1 << ADPS1);
}

int16_t take_averaged_sample(int16_t channel)
{
    #define NUM_SAMPLES_PER_AVERAGE 20
    int16_t sum = 0;
    
    enable_adc_and_set_channel(channel);
    
    for ( int i = 0; i < NUM_SAMPLES_PER_AVERAGE; ++i )
    {
        sum += take_sample();
    }
       
    return sum / NUM_SAMPLES_PER_AVERAGE;
}