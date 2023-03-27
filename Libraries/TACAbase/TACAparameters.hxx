#ifndef TACAPARAMETERS_H
#define TACAPARAMETERS_H

#define TRIG_HEADER 0x00002354       ///< trigger header
#define TRGI_BANK_HEADER 0x49475254  ///< trigger bank header
#define TGEN_BANK_HEADER 0x4e454754  ///< trigger number bank header
#define TRGC_BANK_HEADER 0x43475254  ///< trigger counter bank header

#define NMAXTRIG 64                  ///< maximum number of triggers
#define NMONTRIG 14                  ///< number of mon trigger
#define NCLK 32                      ///< number of clocks

#define MAX_ANUMBER 18                ///< max atomic number

// CA parameters
enum CAparam {
   MaxCry = 333
};

#endif
