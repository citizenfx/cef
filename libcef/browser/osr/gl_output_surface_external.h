// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_OSR_GL_OUTPUT_SURFACE_EXTERNAL_H_
#define CEF_OSR_GL_OUTPUT_SURFACE_EXTERNAL_H_

#include <memory>

#include "cef/libcef/browser/osr/external_renderer_updater.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "components/viz/common/frame_sinks/begin_frame_source.h"
#include "components/viz/service/display_embedder/gl_output_surface.h"
#include "components/viz/service/display_embedder/viz_process_context_provider.h"
#include "components/viz/service/viz_service_export.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "ui/gfx/color_space.h"
#include "ui/gl/gl_export.h"
#include "ui/gl/vsync_observer.h"

namespace gl {
class VSyncThreadWin;
}

namespace viz {

class ExternalImageData;

class VIZ_SERVICE_EXPORT GLOutputSurfaceExternal : public GLOutputSurface, public gl::VSyncObserver {
 public:
  explicit GLOutputSurfaceExternal(
      scoped_refptr<VizProcessContextProvider> context_provider,
      gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
      mojo::Remote<viz::mojom::ExternalRendererUpdater> external_renderer_updater);
  ~GLOutputSurfaceExternal() override;

  // OutputSurface implementation.
  void EnsureBackbuffer() override;
  void DiscardBackbuffer() override;
  void BindFramebuffer() override;
  void Reshape(const ReshapeParams& params) override;
  void SwapBuffers(OutputSurfaceFrame frame) override;
  void SetGpuVSyncCallback(GpuVSyncCallback callback) override;

  void OnVSync(base::TimeTicks timebase, base::TimeDelta interval) override;

 private:
  void HandleVSyncOnMainThread(base::TimeTicks timebase, base::TimeDelta interval);
  void OnSyncWaitComplete(std::vector<ui::LatencyInfo> latency_info);
  void OnAfterSwap(std::vector<ui::LatencyInfo> latency_info);
  void BindTexture();
  void UnbindTexture();
  void ResetBuffer();

  ExternalImageData* CreateSurface();

  const raw_ptr<gl::VSyncThreadWin> vsync_thread_;

  GpuVSyncCallback gpu_vsync_callback_;
  std::unique_ptr<ExternalImageData> current_surface_;
  bool new_texture = false;

  uint32_t fbo_ = 0;
  gfx::Size size_;
  gfx::ColorSpace color_space_;

  base::WeakPtrFactory<GLOutputSurfaceExternal> weak_ptr_factory_{this};

  gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager_;

  mojo::Remote<viz::mojom::ExternalRendererUpdater> external_renderer_updater_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;

  GLOutputSurfaceExternal(const GLOutputSurfaceExternal&);
  void operator=(const GLOutputSurfaceExternal&);
};

}  // namespace viz

#endif  // CEF_OSR_GL_OUTPUT_SURFACE_OFFSCREEN_H_