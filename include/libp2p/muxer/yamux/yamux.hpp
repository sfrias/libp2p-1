/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_YAMUX_IMPL_HPP
#define LIBP2P_YAMUX_IMPL_HPP

#include <libp2p/muxer/muxed_connection_config.hpp>
#include <libp2p/muxer/muxer_adaptor.hpp>

namespace libp2p::muxer {
  class Yamux : public MuxerAdaptor {
   public:
    ~Yamux() override = default;

    /**
     * Create a muxer with Yamux protocol
     * @param config of muxers to be created over the connections
     */
    explicit Yamux(MuxedConnectionConfig config);

    peer::Protocol getProtocolId() const noexcept override;

    void muxConnection(std::shared_ptr<connection::SecureConnection> conn,
                       CapConnCallbackFunc cb) const override;

   private:
    MuxedConnectionConfig config_;
  };
}  // namespace libp2p::muxer

#endif  // LIBP2P_YAMUX_IMPL_HPP
