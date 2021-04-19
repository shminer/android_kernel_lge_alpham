/*
*****************************************************************************
* Copyright by ams AG                                                       *
* All rights are reserved.                                                  *
*                                                                           *
* IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
* THE SOFTWARE.                                                             *
*                                                                           *
* THIS SOFTWARE IS PROVIDED FOR USE ONLY IN CONJUNCTION WITH AMS PRODUCTS.  *
* USE OF THE SOFTWARE IN CONJUNCTION WITH NON-AMS-PRODUCTS IS EXPLICITLY    *
* EXCLUDED.                                                                 *
*                                                                           *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
*****************************************************************************
*/

/*
 * Device Algorithm ALS
 */

/*
 * @@AMS_REVISION_Id: 94092f58014f4160671b0fcf30e2d665c6fbc02a
 */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
#include "ams_als_API.h"
#include "ams_als_core.h"

int amsAlg_als_processData(amsAlsContext_t * ctx, amsAlsDataSet_t * inputData){
    int ret = 0;

    if (inputData->status & ALS_STATUS_RDY){
        ctx->previousLux = ctx->results.mLux;
        if (ctx->previousGain != ctx->gain){
            als_update_statics(ctx);
        } else {
            ctx->notStableMeasurement = false;
        }
#if defined(CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB) || defined(CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND)
        als_compute_data(ctx, inputData);
#endif
#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS_CLEAR
        als_compute_data_clear(ctx, inputData);
#endif
        als_ave_LUX(ctx);
    }

    if (ctx->adaptive){
        if (inputData->status & ALS_STATUS_OVFL){
            ctx->results.adaptive = ADAPTIVE_ALS_GAIN_DEC_REQUEST;
        } else {
            ctx->results.adaptive = als_adaptive(ctx, inputData);
        }
    } else {
        ctx->results.adaptive = ADAPTIVE_ALS_NO_REQUEST;
    }

    if (ctx->notStableMeasurement) {
        ctx->results.mLux = ctx->previousLux;
    }
    return ret;
}
#endif
