// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "lib/jxl/render_pipeline/stage_write_to_ib.h"

#include "lib/jxl/common.h"
#include "lib/jxl/image_bundle.h"

namespace jxl {

namespace {
class WriteToImageBundleStage : public RenderPipelineStage {
 public:
  explicit WriteToImageBundleStage(ImageBundle* image_bundle)
      : RenderPipelineStage(RenderPipelineStage::Settings()),
        image_bundle_(image_bundle) {}

  void ProcessRow(const RowInfo& input_rows, const RowInfo& output_rows,
                  size_t xextra, size_t xsize, size_t xpos, size_t ypos,
                  float* JXL_RESTRICT temp) const final {
    for (size_t c = 0; c < 3; c++) {
      memcpy(image_bundle_->color()->PlaneRow(c, ypos) + xpos - xextra,
             GetInputRow(input_rows, c, 0) + kRenderPipelineXOffset - xextra,
             sizeof(float) * (xsize + 2 * xextra));
    }
    for (size_t ec = 0; ec < image_bundle_->extra_channels().size(); ec++) {
      JXL_ASSERT(ec < image_bundle_->extra_channels().size());
      JXL_ASSERT(image_bundle_->extra_channels()[ec].xsize() <=
                 xpos + xsize + xextra);
      memcpy(
          image_bundle_->extra_channels()[ec].Row(ypos) + xpos - xextra,
          GetInputRow(input_rows, 3 + ec, 0) + kRenderPipelineXOffset - xextra,
          sizeof(float) * (xsize + 2 * xextra));
    }
  }

  RenderPipelineChannelMode GetChannelMode(size_t c) const final {
    return c < 3 + image_bundle_->extra_channels().size()
               ? RenderPipelineChannelMode::kInput
               : RenderPipelineChannelMode::kIgnored;
  }

 private:
  ImageBundle* image_bundle_;
};

}  // namespace

std::unique_ptr<RenderPipelineStage> GetWriteToImageBundleStage(
    ImageBundle* image_bundle) {
  return jxl::make_unique<WriteToImageBundleStage>(image_bundle);
}
}  // namespace jxl
