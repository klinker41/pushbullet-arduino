/*
 * Copyright (C) 2015 Jacob Klinker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 */

package com.klinker.web.pushbullet

import grails.converters.JSON
import groovyx.net.http.ContentType
import groovyx.net.http.RESTClient

/**
 * Forwards information on pushes to Pushbullet through https://api.pushbullet.com/v2/pushes
 */
class PushesController {

    static allowedMethods = [add:'DELETE']

    private static final int LIMIT = 5

    /**
     * Find a JSON of the current user object's pushes based off of authorization header access token. More
     * information can be found at https://docs.pushbullet.com/#pushes.
     *
     * NOTE: we will only be returning the last 15 pushes in this method. This is sufficient for a proof of concept and
     * the more data that we get, the more difficult it will be on the Arduino
     *
     * @return JSON containing current user's pushes
     */
    def index() {
        def client = new RESTClient(PushbulletApi.BASE_URL)
        def resp

        if (request.get) {
            // GET request, returns last 15 pushes
            resp = client.get(
                    path: 'v2/pushes',
                    headers: ['Authorization': request.getHeader('Authorization')],
                    requestContentType: ContentType.JSON,
                    query: [
                            modifiedAfter: '0',
                            active: 'true',
                            limit: LIMIT
                    ]
            )
        } else if (request.post) {
            resp = client.post(
                    path: 'v2/pushes',
                    headers: ['Authorization': request.getHeader('Authorization')],
                    requestContentType: ContentType.JSON,
                    body: request.getJSON()
            )
        } else {
            def id = params.id

            if (id == null) {
                // DELETE request, deletes all pushes, careful!!
                resp = client.delete(
                        path: 'v2/pushes',
                        headers: ['Authorization': request.getHeader('Authorization')],
                        requestContentType: ContentType.JSON,
                )
            } else {
                // DELETE request, deletes a single push
                resp = client.delete(
                        path: 'v2/pushes/' + id,
                        headers: ['Authorization': request.getHeader('Authorization')],
                        requestContentType: ContentType.JSON,
                )
            }
        }

        assert resp.status == 200
        render resp.getData() as JSON
        return resp.getData()
    }

}
