/**
 * @file io/cxx/VideoReader.cc
 * @date Wed Jun 22 17:50:08 2011 +0200
 * @author Andre Anjos <andre.anjos@idiap.ch>
 *
 * @brief A class to help you read videos. This code originates from
 * http://ffmpeg.org/doxygen/1.0/, "decoding & encoding example".
 *
 * Copyright (C) 2011-2013 Idiap Research Institute, Martigny, Switzerland
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bob/io/VideoReader.h"

#include <stdexcept>
#include <boost/format.hpp>
#include <boost/preprocessor.hpp>
#include <limits>

#include "bob/core/array_check.h"
#include "bob/core/blitz_array.h"
#include "bob/core/logging.h"

#ifndef AV_PIX_FMT_RGB24
#define AV_PIX_FMT_RGB24 PIX_FMT_RGB24
#endif

namespace io = bob::io;
namespace ffmpeg = bob::io::detail::ffmpeg;

io::VideoReader::VideoReader(const std::string& filename) {
  open(filename);
}

io::VideoReader::VideoReader(const io::VideoReader& other) {
  *this = other;
}

io::VideoReader& io::VideoReader::operator= (const io::VideoReader& other) {
  open(other.filename());
  return *this;
}

void io::VideoReader::open(const std::string& filename) {
  m_filepath = filename;

  boost::shared_ptr<AVFormatContext> format_ctxt =
    ffmpeg::make_input_format_context(m_filepath);

  m_formatname = format_ctxt->iformat->name;
  m_formatname_long = format_ctxt->iformat->long_name;

  int stream_index = ffmpeg::find_video_stream(m_filepath, format_ctxt);

  AVCodec* codec = ffmpeg::find_decoder(m_filepath, format_ctxt, stream_index);
  
  m_codecname = codec->name;
  m_codecname_long = codec->long_name;

  boost::shared_ptr<AVCodecContext> codec_ctxt = 
    ffmpeg::make_codec_context(m_filepath, 
        format_ctxt->streams[stream_index], codec);

  /**
   * Copies some information from the context just opened
   */
  m_width = codec_ctxt->width;
  m_height = codec_ctxt->height;
  m_duration = format_ctxt->duration;
  m_nframes = format_ctxt->streams[stream_index]->nb_frames;
  if (m_nframes > 0) {
    //number of frames is known
    m_framerate = m_nframes * AV_TIME_BASE / m_duration;
  }
  else {
    //number of frames is not known
    m_framerate = av_q2d(format_ctxt->streams[stream_index]->r_frame_rate);
    m_nframes = (int)(m_framerate * m_duration / AV_TIME_BASE);
  }

  /**
   * This will create a local description of the contents of the stream, in
   * printable format.
   */
  boost::format fmt("Video file: %s; FFmpeg: avformat-%s; avcodec-%s; avutil-%s; swscale-%d; Format: %s (%s); Codec: %s (%s); Time: %.2f s (%d @ %2.fHz); Size (w x h): %d x %d pixels");
  fmt % m_filepath;
  fmt % BOOST_PP_STRINGIZE(LIBAVFORMAT_VERSION);
  fmt % BOOST_PP_STRINGIZE(LIBAVCODEC_VERSION);
  fmt % BOOST_PP_STRINGIZE(LIBAVUTIL_VERSION);
  fmt % BOOST_PP_STRINGIZE(LIBSWSCALE_VERSION);
  fmt % m_formatname_long;
  fmt % m_formatname;
  fmt % m_codecname_long;
  fmt % m_codecname;
  fmt % (m_duration / 1e6);
  fmt % m_nframes;
  fmt % m_framerate;
  fmt % m_width;
  fmt % m_height;
  m_formatted_info = fmt.str();

  /**
   * This will make sure we can interface with the io subsystem
   */
  m_typeinfo_video.dtype = m_typeinfo_frame.dtype = bob::core::array::t_uint8;
  m_typeinfo_video.nd = 4;
  m_typeinfo_frame.nd = 3;
  m_typeinfo_video.shape[0] = m_nframes;
  m_typeinfo_video.shape[1] = m_typeinfo_frame.shape[0] = 3;
  m_typeinfo_video.shape[2] = m_typeinfo_frame.shape[1] = m_height;
  m_typeinfo_video.shape[3] = m_typeinfo_frame.shape[2] = m_width;
  m_typeinfo_frame.update_strides();
  m_typeinfo_video.update_strides();

}

io::VideoReader::~VideoReader() {
}

size_t io::VideoReader::load(blitz::Array<uint8_t,4>& data, 
  bool throw_on_error) const {
  bob::core::array::blitz_array tmp(data);
  return load(tmp, throw_on_error);
}

size_t io::VideoReader::load(bob::core::array::interface& b, 
  bool throw_on_error) const {

  //checks if the output array shape conforms to the video specifications,
  //otherwise, throw.
  if (!m_typeinfo_video.is_compatible(b.type())) {
    boost::format s("input buffer (%s) does not conform to the video size specifications (%s)");
    s % b.type().str() % m_typeinfo_video.str();
    throw std::invalid_argument(s.str());
  }

  unsigned long int frame_size = m_typeinfo_frame.buffer_size();
  uint8_t* ptr = static_cast<uint8_t*>(b.ptr());
  size_t frames_read = 0;

  for (const_iterator it=begin(); it!=end();) {
    bob::core::array::blitz_array ref(static_cast<void*>(ptr), m_typeinfo_frame);
    if (it.read(ref, throw_on_error)) {
      ptr += frame_size;
      ++frames_read;
    }
    //otherwise we don't count!
  }

  return frames_read;
}

io::VideoReader::const_iterator io::VideoReader::begin() const {
  return io::VideoReader::const_iterator(this);
}

io::VideoReader::const_iterator io::VideoReader::end() const {
  return io::VideoReader::const_iterator();
}

io::VideoReader::const_iterator::const_iterator(const io::VideoReader* parent) :
  m_parent(parent),
  m_current_frame(std::numeric_limits<size_t>::max())
{
  init();
}

io::VideoReader::const_iterator::const_iterator():
  m_parent(0),
  m_current_frame(std::numeric_limits<size_t>::max())
{
}

io::VideoReader::const_iterator::const_iterator
(const io::VideoReader::const_iterator& other) :
  m_parent(other.m_parent),
  m_current_frame(std::numeric_limits<size_t>::max())
{
  init();
  (*this) += other.m_current_frame;
}

io::VideoReader::const_iterator::~const_iterator() {
  reset();
}

io::VideoReader::const_iterator& io::VideoReader::const_iterator::operator= (const io::VideoReader::const_iterator& other) {
  reset();
  m_parent = other.m_parent;
  init();
  (*this) += other.m_current_frame;
  return *this;
}

void io::VideoReader::const_iterator::init() {

  //ffmpeg initialization
  const std::string& filename = m_parent->filename();
  m_format_context = ffmpeg::make_input_format_context(filename);
  m_stream_index = ffmpeg::find_video_stream(filename, m_format_context);
  m_codec = ffmpeg::find_decoder(filename, m_format_context, m_stream_index);
  m_codec_context = ffmpeg::make_codec_context(filename, 
        m_format_context->streams[m_stream_index], m_codec);
  m_swscaler = ffmpeg::make_scaler(filename, m_codec_context,
      m_codec_context->pix_fmt, PIX_FMT_RGB24);
  m_context_frame = ffmpeg::make_empty_frame(filename);
  m_rgb_array.reference(blitz::Array<uint8_t,3>(m_codec_context->height, 
      m_codec_context->width, 3));

  //at this point we are ready to start reading out frames.
  m_current_frame = 0;
  
  //the file maybe valid, but contain zero frames... We check for this here:
  if (m_current_frame >= m_parent->numberOfFrames()) {
    //transforms the current iterator in "end"
    reset();
  }

}

void io::VideoReader::const_iterator::reset() {
  m_context_frame.reset();
  m_swscaler.reset();
  m_codec_context.reset();
  m_codec = 0;
  m_format_context.reset();
  m_current_frame = std::numeric_limits<size_t>::max(); //that means "end" 
  m_parent = 0;
}

bool io::VideoReader::const_iterator::read(blitz::Array<uint8_t,3>& data,
  bool throw_on_error) {
  bob::core::array::blitz_array tmp(data);
  return read(tmp, throw_on_error);
}

bool io::VideoReader::const_iterator::read(bob::core::array::interface& data,
  bool throw_on_error) {

  if (!m_parent) {
    //we are already past the end of the stream
    throw std::runtime_error("video iterator for file has already reached its end and was reset");
  }

  //checks if we have not passed the end of the video sequence already
  if(m_current_frame >= m_parent->numberOfFrames()) {

    if (throw_on_error) {
      boost::format m("you are trying to read past the file end (next frame no. to be read would be %d) on file %s, which contains only %d frames");
      m % m_current_frame % m_parent->m_filepath % m_parent->m_nframes;
      throw std::runtime_error(m.str());
    }
    
    reset();
    return false;
  }

  const bob::core::array::typeinfo& info = data.type();

  //checks if the output array shape conforms to the video specifications,
  //otherwise, throw
  if (!info.is_compatible(m_parent->m_typeinfo_frame)) {
    boost::format s("input buffer (%s) does not conform to the video frame size specifications (%s)");
    s % info.str() % m_parent->m_typeinfo_frame.str();
    throw std::invalid_argument(s.str());
  }

  //we are going to need another copy step - use our internal array
  bool ok = ffmpeg::read_video_frame(m_parent->m_filepath, m_current_frame,
      m_stream_index, m_format_context, m_codec_context, m_swscaler,
      m_context_frame, m_rgb_array.data(), throw_on_error);

  if (ok) {

    //now we copy from one container to the other, using our Blitz++ technique
    blitz::TinyVector<int,3> shape;
    blitz::TinyVector<int,3> stride;

    shape = info.shape[0], info.shape[1], info.shape[2];
    stride = info.stride[0], info.stride[1], info.stride[2];
    blitz::Array<uint8_t,3> dst(static_cast<uint8_t*>(data.ptr()), 
        shape, stride, blitz::neverDeleteData);

    dst = m_rgb_array.transpose(2,0,1);
    ++m_current_frame;

  }

  return ok;
}

/**
 * This method does essentially the same as read(), except it skips a few
 * operations to get a better performance.
 */
io::VideoReader::const_iterator& io::VideoReader::const_iterator::operator++ () {
  if (!m_parent) {
    //we are already past the end of the stream
    throw std::runtime_error("video iterator for file has already reached its end and was reset");
  }

  //checks if we have not passed the end of the video sequence already
  if(m_current_frame >= m_parent->numberOfFrames()) {
    reset();
    return *this;
  }

  //we are going to need another copy step - use our internal array
  try {
    bool ok = ffmpeg::skip_video_frame(m_parent->m_filepath, m_current_frame,
        m_stream_index, m_format_context, m_codec_context, m_context_frame,
        true);
    if (ok) ++m_current_frame;
  }
  catch (std::runtime_error& e) {
    reset();
  }

  return *this;
}

io::VideoReader::const_iterator& io::VideoReader::const_iterator::operator+= (size_t frames) {
  for (size_t i=0; i<frames; ++i) ++(*this);
  return *this;
}

bool io::VideoReader::const_iterator::operator== (const const_iterator& other) {
  return (this->m_parent == other.m_parent) && (this->m_current_frame == other.m_current_frame);
}

bool io::VideoReader::const_iterator::operator!= (const const_iterator& other) {
  return !(*this == other);
}
