#ifndef __wdg_H
#define __wdg_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

void WDG_Init(void);

void WDG_Reset(void);

void WDG_Service(void);


#ifdef __cplusplus
}
#endif
#endif /*__ wdg_H */
