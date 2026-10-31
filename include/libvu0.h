#ifndef LIBVU0_H
#define LIBVU0_H

/*
 * VU0 macro-mode vector and matrix library (libvu0). Only what FFX is shown to use.
 *
 * The SDK 2.0 libvu0.c, compiled with ee-gcc2.9-991111-01-dtls13010 -O2 -G0 -fno-common, matches
 * FFX's 0x2DCA98-0x2DD7D4 as a whole object, so these are Sony's own names and signatures. The
 * functions load their operands with lqc2/sqc2, which need the 16-byte alignment of the types.
 */

typedef int sceVu0IVECTOR[4] __attribute__((aligned(16)));
typedef float sceVu0FVECTOR[4] __attribute__((aligned(16)));
typedef float sceVu0FMATRIX[4][4] __attribute__((aligned(16)));

void sceVu0CopyVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1);                                           /* 0x2DCD40 */
void sceVu0CopyVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1);                                        /* 0x2DD5D8 */
void sceVu0FTOI4Vector(sceVu0IVECTOR v0, sceVu0FVECTOR v1);                                          /* 0x2DCD78 */
void sceVu0ScaleVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float s);                                 /* 0x2DCCF8 */
void sceVu0AddVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);                          /* 0x2DCCB0 */
void sceVu0SubVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);                          /* 0x2DCCC8 */
void sceVu0MulVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);                          /* 0x2DCCE0 */
void sceVu0InterVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float r);               /* 0x2DCC88 */
void sceVu0InterVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2, float r);            /* 0x2DD5F8 */
void sceVu0DivVector(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q);                                   /* 0x2DCC48 */
void sceVu0DivVectorXYZ(sceVu0FVECTOR v0, sceVu0FVECTOR v1, float q);                                /* 0x2DCC68 */
float sceVu0InnerProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1);                                        /* 0x2DCB30 */
void sceVu0OuterProduct(sceVu0FVECTOR v0, sceVu0FVECTOR v1, sceVu0FVECTOR v2);                       /* 0x2DCB10 */
void sceVu0Normalize(sceVu0FVECTOR v0, sceVu0FVECTOR v1);                                            /* 0x2DCB58 */
void sceVu0ApplyMatrix(sceVu0FVECTOR v0, sceVu0FMATRIX m, sceVu0FVECTOR v1);                         /* 0x2DCA98 */
void sceVu0UnitMatrix(sceVu0FMATRIX m);                                                              /* 0x2DCDB8 */
void sceVu0CopyMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);                                           /* 0x2DCD50 */
void sceVu0TransposeMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);                                      /* 0x2DCB90 */
void sceVu0MulMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FMATRIX m2);                          /* 0x2DCAC8 */
void sceVu0InversMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1);                                         /* 0x2DCBD8 */
void sceVu0RotMatrixX(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rx);                                 /* 0x2DCF00 */
void sceVu0RotMatrixY(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float ry);                                 /* 0x2DCFA8 */
void sceVu0RotMatrixZ(sceVu0FMATRIX m0, sceVu0FMATRIX m1, float rz);                                 /* 0x2DCE58 */
void sceVu0RotMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR rot);                         /* 0x2DD050 */
void sceVu0TransMatrix(sceVu0FMATRIX m0, sceVu0FMATRIX m1, sceVu0FVECTOR tv);                        /* 0x2DCD10 */
void sceVu0CameraMatrix(sceVu0FMATRIX m, sceVu0FVECTOR p, sceVu0FVECTOR zd, sceVu0FVECTOR yd);       /* 0x2DD0C8 */
void sceVu0NormalLightMatrix(sceVu0FMATRIX m, sceVu0FVECTOR l0, sceVu0FVECTOR l1, sceVu0FVECTOR l2); /* 0x2DD178 */
void sceVu0LightColorMatrix(
    sceVu0FMATRIX m, sceVu0FVECTOR c0, sceVu0FVECTOR c1, sceVu0FVECTOR c2, sceVu0FVECTOR a); /* 0x2DD238 */
void sceVu0ViewScreenMatrix(sceVu0FMATRIX m, float scrz, float ax, float ay, float cx, float cy, float zmin, float zmax,
                            float nearz, float farz);                                                /* 0x2DD2A0 */
void sceVu0DropShadowMatrix(sceVu0FMATRIX m, sceVu0FVECTOR lp, float a, float b, float c, int mode); /* 0x2DD3A8 */
int sceVu0ClipScreen(sceVu0FVECTOR v0);                                                              /* 0x2DD640 */
void sceVu0RotTransPers(sceVu0IVECTOR v0, sceVu0FMATRIX m0, sceVu0FVECTOR v1, int mode);             /* 0x2DD590 */
void sceVpu0Reset(void);                                                                             /* 0x2DD778 */

#endif
